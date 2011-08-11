
#include <cassert>
#include <utility>

#include <cinder/Rand.h>
#include <cinder/app/App.h>

#include <graph/graph_ParticleSystem.h>

#include <adj/adj_DJController.h>
#include <adj/adj_GraphPhysics.h>
#include <adj/adj_PlayManager.h>
#include <adj/adj_GraphNodeFactory.h>
#include <adj/adj_GraphNode.h>
#include <adj/adj_CalloutBox.h>
#include <adj/adj_User.h>

namespace adj {

DJController* DJController::instance_ = NULL;

DJController::DJController() {
    has_next_song_ = false;
    transition_next_loop_ = false;
    counter_ = 0; 
}

void DJController::init() {
    // nothing here
}

void DJController::transition() {
    if (!has_next_song_)
        set_next_song_randomly();

    // return if next song == now playing

    GraphNodePtr old_song = PlayManager::instance().now_playing();

    if (old_song->song().id() == next_song_)
        set_next_song_randomly();

    // delete all attractions
    GraphPhysics::instance().clear_all_node_connections();

    // delete now playing node
    GraphNodeFactory::instance().delete_node(old_song);

    // make sure this method works
    assert(old_song.unique()); 

    // this call should actually delete the object
    old_song.reset();

    // update now playing to new node
    PlayManager::instance().set_now_playing(
        GraphNodeFactory::instance().song_map()[next_song_]);

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

    now_playing->set_is_current_song(true);
    // tell the node to fade in
    now_playing->callout_box_->show();
    now_playing->register_song_playing();

    std::vector<GraphNodePtr>& nodes = GraphNodeFactory::instance().nodes();
	
	std::pair<GraphNodePtr, GraphNodePtr> temp_pair;

    for (std::vector<GraphNodePtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        if (*it == now_playing)
            continue;

        GraphPhysics::instance().link_nodes(now_playing, *it);
		
		temp_pair = std::pair<GraphNodePtr, GraphNodePtr>(now_playing, *it);
        GraphNodeFactory::instance().add_edge(temp_pair);
    }

    // add edges

    has_next_song_ = false;
}

void DJController::set_next_song(SongId id) {
    next_song_ = id;
    has_next_song_ = true;
}

void DJController::set_next_song_randomly() {
    ci::Rand rand;
    rand.randomize();

    SongId possible_id;

    int num_nodes = GraphNodeFactory::instance().nodes().size();

    do {
        possible_id = GraphNodeFactory::instance().nodes()[rand.randInt(num_nodes)]->song().id();
    } while (possible_id == PlayManager::instance().now_playing()->song().id() || 
        (has_next_song_ && possible_id == next_song_));

    next_song_ = possible_id;
    has_next_song_ = true;
}
    
void DJController::update() {
	if (transition_next_loop_ && GraphNodeFactory::instance().pair_requests().empty()
        && !UserFactory::instance().has_pending_queries()) {
        
        if (counter_ > 0) {
    		transition();
        	transition_next_loop_ = false;
            counter_ = 0;
        } else {
            ++counter_;
        }

    }
}

DJController& DJController::instance() {
    if (instance_ == NULL) {
        instance_ = new DJController();
        instance_->init();
    }

    return *instance_;
}




}