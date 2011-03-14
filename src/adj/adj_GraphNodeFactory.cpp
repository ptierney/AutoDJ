
#include <assert.h>

#include <map>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <cinder/Rand.h>
#include <cinder/app/App.h>

#include <graph/graph_ParticleSystem.h>
#include <graph/graph_Particle.h>

#include <adj/adj_GraphNodeFactory.h>
#include <adj/adj_GraphNode.h>
#include <adj/adj_GraphPhysics.h>
#include <adj/adj_PlayManager.h>
#include <adj/adj_Song.h>
#include <adj/adj_VoteManager.h>
#include <adj/adj_GraphOracle.h>
#include <adj/adj_NowPlayingHeadline.h>
#include <adj/adj_DJController.h>

namespace adj {

GraphNodeFactory::GraphNodeFactory() {
    max_nodes_in_simulation_ = 25;
    sim_counter_ = 0;
}

void GraphNodeFactory::init() {
    // nothnig here
}

void GraphNodeFactory::update() {
    check_pair_requests();
    
    ci::Rand rand;
    rand.randomize();
    
    float r_scale = 0.7f;
    
    if (sim_counter_ > 150) {
        sim_counter_ = 0;
    
    for (std::vector<GraphNodePtr>::iterator it = graph_nodes_.begin();
         it != graph_nodes_.end(); ++it) {
         
         (*it)->particle()->position().x += rand.randFloat(-r_scale, r_scale);
         (*it)->particle()->position().y += rand.randFloat(-r_scale, r_scale);
    }
    
    }
    sim_counter_++;
}

void GraphNodeFactory::add_empty_node() {
    add_new_to_node(GraphNodePtr());
}

void GraphNodeFactory::add_new_to_random_node() {
    if (graph_nodes_.empty()) {
        add_empty_node();
        return;
    }

    ci::Rand rand;
    rand.randomize();

    GraphNodePtr p = graph_nodes_[rand.randInt(graph_nodes_.size() - 1)];

    add_new_to_node(p);
}

void GraphNodeFactory::add_new_to_node(GraphNodePtr p) {
    GraphNodePtr q = create_new_node(SongFactory::instance().get_random_song_id());

    connect_nodes(q, p);

    q->show();

    PlayManager::instance().register_new_graph_node(q);
}

GraphNodePtr GraphNodeFactory::create_new_node(SongId id) {
    GraphNodePtr q(new GraphNode());
    q->song_ = SongFactory::instance().lookup_song(id);
	
    q->init();

    graph_nodes_.push_back(q);
    song_map_[q->song().id()] = q;

    PlayManager::instance().register_new_graph_node(q);

    return q;
}

void GraphNodeFactory::add_node_to_physics(GraphNodePtr a) {
    connect_nodes(a, GraphNodePtr());
}

// what the hell's the difference between connect_nodes and pair_nodes
void GraphNodeFactory::connect_nodes(GraphNodePtr a, GraphNodePtr b) {
    if (b.get() != NULL) {
        a->parent_ = b;
        a->particle_ = GraphPhysics::instance().create_particle(b->particle_);
        b->add_child(a);
    } else {
        a->particle_ = GraphPhysics::instance().create_unconnected_particle();
        ci::app::console() << "**WARNING** Adding loose node to system" << std::endl;
    }
}

void GraphNodeFactory::assert_node_has_particle(GraphNodePtr p) {
    if (p->particle_.get() != NULL)
        return;

    p->particle_ = GraphPhysics::instance().create_unconnected_particle();
}

void GraphNodeFactory::update_graph_from_vote(VotePtr vote) {
    SongId song_id = vote->song_id;

    // if the size of the graph nodes is too big,
    // delete on of the nodes
    // don't delete the now playing node, or the node 
    // that this  would add to
    
    if (graph_nodes_.size() > max_nodes_in_simulation_) {
        int max_votes = 0;
        SongId now_playing_id = PlayManager::instance().now_playing()->song().id();
        SongId next_id = DJController::instance().next_song();
        bool has_next_song = DJController::instance().has_next_song();   
        
        GraphNodePtr to_delete;

        // find the node with the least number of votes
        for (std::vector<GraphNodePtr>::iterator it = graph_nodes_.begin();
            it != graph_nodes_.end(); ++it) {
            
            SongId id = (*it)->song().id();
            
            if (id == now_playing_id || id == song_id || 
                (has_next_song && id == next_id))
                continue;
            
            if ((*it)->song().num_votes() < max_votes)
                continue;
                
            max_votes = (*it)->song().num_votes();
            to_delete = *it;
        }
        
        // properly delete node using 
        
        delete_node_and_connections(to_delete);
    }
    
    
    // search through nodes for song id 
    song_map_it_ = song_map_.find(song_id);
    if (song_map_it_ != song_map_.end()) {
        song_map_it_->second->register_vote(vote);
        return;
    }

    // need to create a new node
    create_new_node_from_vote(vote);
}

void GraphNodeFactory::create_new_node_from_vote(VotePtr vote) {
    // create a new node. This doesn't create a particle
    GraphNodePtr new_node;
	
	try {
		new_node = create_new_node(vote->song_id);
	} catch (std::exception ex) {
		ci::app::console() << "Error creating new node from vote." << std::endl;
		ci::app::console() << ex.what() << std::endl;
		return;
	}

    ci::app::console() << "Creating new node for song: " << 
        new_node->song().name() << std::endl;
	
	new_node->register_vote(vote);

    // if it's the first, show it, return
    if (graph_nodes_.size() == 1) {
        add_node_to_physics(new_node);
        new_node->show();
        return;
    }

    // node is now hidden

    // query the oracle for the node's position in the graph
    GraphOracle::instance().query_best_connection(vote->song_id);

    // when the query comes back, pair the node and show it.
}

void GraphNodeFactory::pair_nodes(SongId new_id, SongId existing_id) {
    GraphNodePtr new_node = song_map_[new_id];
    GraphNodePtr existing_node = song_map_[existing_id];

    assert(new_node.get() != NULL && existing_node.get() != NULL);

    if (new_node->particle().get() == NULL)
        new_node->particle_ = 
            GraphPhysics::instance().create_unlinked_particle(existing_node->particle());

    GraphPhysics::instance().link_nodes(new_node, existing_node);

    existing_node->add_child(new_node);
    new_node->parent_ = existing_node;

	std::pair<GraphNodePtr, GraphNodePtr> p(new_node, existing_node);
    add_edge(p);

    new_node->register_just_added();
    new_node->show();
    existing_node->show();
}

void GraphNodeFactory::add_pair_request(PairRequest request) {
    boost::mutex::scoped_lock lock(pair_requests_mutex_);

    pair_requests_.push_back(request);
}

void GraphNodeFactory::check_pair_requests() {
    boost::mutex::scoped_lock lock(pair_requests_mutex_);

    if (pair_requests_.empty())
        return;

    for (std::deque<PairRequest>::iterator it = pair_requests_.begin();
        it != pair_requests_.end(); ++it) {

         pair_nodes(it->first, it->second);
    }

    pair_requests_.clear();
}

void GraphNodeFactory::add_edge(std::pair<GraphNodePtr, GraphNodePtr>& edge) {
    edges_.push_back(edge);
}

void GraphNodeFactory::remove_edge(std::pair<GraphNodePtr, GraphNodePtr>& edge) {
    for (std::vector<std::pair<GraphNodePtr, GraphNodePtr> >::iterator
        it = edges_.begin(); it != edges_.end(); ) {
        
        if ( (it->first == edge.first && it->second == edge.second) ||
            (it->first == edge.second && it->second == edge.first) ) {

            edges_.erase(it);
            it = edges_.begin();
            continue;
        }

        ++it;
    }
}

void GraphNodeFactory::delete_node_and_connections(GraphNodePtr& node) {
    // delete all attractions
    GraphPhysics::instance().clear_all_node_connections();
    
    delete_node(node);
    
    assert(node.unique());
    
    node.reset();
    
    // make "choose 2" separations between nodes
    ParticleSystemPtr system = GraphPhysics::instance().particle_system();
    std::vector<ParticlePtr>& particles = system->particles_;

    for (std::vector<ParticlePtr>::iterator it_a = particles.begin();
        it_a != particles.end(); ++it_a) {

        for (std::vector<ParticlePtr>::iterator it_b = particles.begin();
            it_b != particles.end(); ++it_b) {

            if (*it_a == *it_b)
                continue;

            GraphPhysics::instance().make_separation_between_node_particles(*it_a, *it_b);
        }
    }
    
    // make n-1 springs to now playing

    GraphNodePtr now_playing = PlayManager::instance().now_playing();
    
    std::vector<GraphNodePtr>& nodes = GraphNodeFactory::instance().nodes();
	
	std::pair<GraphNodePtr, GraphNodePtr> temp_pair;

    for (std::vector<GraphNodePtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        if (*it == now_playing)
            continue;

        GraphPhysics::instance().link_nodes(now_playing, *it);
		
		temp_pair = std::pair<GraphNodePtr, GraphNodePtr>(now_playing, *it);
        GraphNodeFactory::instance().add_edge(temp_pair);
    }
    
}


void GraphNodeFactory::delete_node(GraphNodePtr node) {
    // remove from main vector
    std::vector<GraphNodePtr>::iterator vec_it = std::find(
        graph_nodes_.begin(), graph_nodes_.end(), node);
    
    graph_nodes_.erase(vec_it);

    // remove from edges

    for (std::vector<std::pair<GraphNodePtr, GraphNodePtr> >::iterator
        it = edges_.begin(); it != edges_.end(); ) {
        
         if (it->first == node || it->second == node) {
            edges_.erase(it);
            it = edges_.begin();
            continue;
        }

        ++it;
    }

    // remove from song map
    song_map_it_ = song_map_.find(node->song().id());

    song_map_.erase(song_map_it_);

    // remove from any nodes's children

    for (std::vector<GraphNodePtr>::iterator it = graph_nodes_.begin();
        it != graph_nodes_.end(); ++it) {

        (*it)->remove_child(node);
    }

    // remove from any node's parents
    for (std::vector<GraphNodePtr>::iterator it = graph_nodes_.begin();
        it != graph_nodes_.end(); ++it) {

        if ((*it)->parent() != node)
             continue;

        (*it)->delete_parent();
    }

    // check the PlayManager

    if (PlayManager::instance().next_song_ == node)
        PlayManager::instance().next_song_.reset();

    if (PlayManager::instance().now_playing_ == node)
        PlayManager::instance().now_playing_.reset();

    // check if it's in the Now Playing Headline

    NowPlayingHeadline::instance().remove_now_playing(node);
}


GraphNodeFactory* GraphNodeFactory::instance_ = NULL;

GraphNodeFactory& GraphNodeFactory::instance() {
    if (instance_ == NULL) {
        instance_ = new GraphNodeFactory();
        instance_->init();
    }

    return *instance_;
}

void GraphNodeFactory::cleanup() {
    if (instance_ == NULL)
        return;

    delete instance_;
}

}
