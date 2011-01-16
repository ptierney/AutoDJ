
#include "json/value.h"

#include "cinder/Rand.h"

#include "adj/adj_GraphNodeFactory.h"
#include "adj/adj_GraphNode.h"
#include "adj/adj_GraphPhysics.h"

namespace adj {

GraphNodeFactory::GraphNodeFactory() {
    // nothing here
}

void GraphNodeFactory::init() {
    // nothnig here
}

void GraphNodeFactory::add_node_from_song_request(const Json::Value&) {

}

void GraphNodeFactory::add_empty_node() {
    GraphNodePtr n(new GraphNode());

    n->particle_ = GraphPhysics::instance().create_particle();
    n->is_valid_ = false; // this is true when it has a song

    graph_nodes_.push_back(n);
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
    q->parent_ = p;
    q->particle_ = GraphPhysics::instance().create_particle(p->particle_);
    p->add_child(q);
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