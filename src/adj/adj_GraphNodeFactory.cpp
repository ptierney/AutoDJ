
#include <assert.h>

#include <map>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"

#include "cinder/Rand.h"
#include "cinder/app/App.h"

#include "adj/adj_GraphNodeFactory.h"
#include "adj/adj_GraphNode.h"
#include "adj/adj_GraphPhysics.h"
#include "adj/adj_PlayManager.h"
#include "adj/adj_Song.h"
#include "adj/adj_VoteManager.h"
#include "adj/adj_GraphOracle.h"

namespace adj {

GraphNodeFactory::GraphNodeFactory() {
    // nothing here
}

void GraphNodeFactory::init() {
    // nothnig here
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

    return q;
}

void GraphNodeFactory::add_node_to_physics(GraphNodePtr a) {
    connect_nodes(a, GraphNodePtr());
}

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
    // create a new node
    GraphNodePtr new_node = create_new_node(vote->song_id);

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

void GraphNodeFactory::pair_nodes(SongId a, SongId b) {
    GraphNodePtr node_a = song_map_[a];
    GraphNodePtr node_b = song_map_[b];

    if (node_a.get() == NULL || node_b.get() == NULL) {
        assert(0); // wtf something's wrong
    }

    assert_node_has_particle(node_a);
    assert_node_has_particle(node_b);

    GraphPhysics::instance().link_nodes(node_a, node_b);

    node_a->show();
    node_b->show();
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