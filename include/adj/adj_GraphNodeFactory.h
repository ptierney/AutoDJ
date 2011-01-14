
#pragma once

#include "adj/adj_Adj.h"

#include <deque>

#include "json/value.h"

namespace graph {
    class ParticleSystem;
    class Particle;
}

namespace adj {

class GraphNode;
typedef std::tr1::shared_ptr<GraphNode> GraphNodePtr;

// creates a node, and adds it to the particle system
class GraphNodeFactory {
public:
    static GraphNodeFactory& instance();
    static void cleanup();

    // parses a request from the network 
    void add_node_from_song_request(const Json::Value&);

    void add_empty_node();

private:
    GraphNodeFactory();
    void init();

    static GraphNodeFactory* instance_;

    std::deque<GraphNodePtr> graph_nodes_;
};

}