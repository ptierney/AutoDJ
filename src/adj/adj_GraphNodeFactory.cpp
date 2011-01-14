
#include "json/value.h"

#include "adj/adj_GraphNodeFactory.h"
#include "adj/adj_GraphNode.h"

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