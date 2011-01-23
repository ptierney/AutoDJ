
#include <map>

#include "cinder/Rand.h"
#include "cinder/app/App.h"

#include "adj/adj_GraphNodeFactory.h"
#include "adj/adj_GraphNode.h"
#include "adj/adj_GraphPhysics.h"
#include "adj/adj_PlayManager.h"
#include "adj/adj_Song.h"
#include "adj/adj_VoteManager.h"

namespace adj {

GraphNodeFactory::GraphNodeFactory() {
    // nothing here
}

void GraphNodeFactory::init() {
    // nothnig here
}

void GraphNodeFactory::add_empty_node() {
    add_to_node(GraphNodePtr());
}

void GraphNodeFactory::add_to_random_node() {
    if (graph_nodes_.empty()) {
        add_empty_node();
        return;
    }

    ci::Rand rand;
    rand.randomize();

    GraphNodePtr p = graph_nodes_[rand.randInt(graph_nodes_.size() - 1)];

    add_to_node(p);
}

void GraphNodeFactory::add_to_node(GraphNodePtr p) {
    GraphNodePtr q(new GraphNode());

    if (p.get() != NULL) {
        q->parent_ = p;
        q->particle_ = GraphPhysics::instance().create_particle(p->particle_);
        p->add_child(q);
    } else {
        q->particle_ = GraphPhysics::instance().create_unconnected_particle();
        ci::app::console() << "**WARNING** Adding loose node to system" << std::endl;
    }

    q->song_ = SongFactory::instance().get_random_song();

    q->init();

    graph_nodes_.push_back(q);
    song_map_[q->song().id()] = q;

    PlayManager::instance().register_new_graph_node(q);
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
    SongId connecting_song = 
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