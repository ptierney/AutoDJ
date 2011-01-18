
#pragma once

#include "adj/adj_Adj.h"

#include <vector>

#include "json/value.h"

namespace graph {
    class ParticleSystem;
    class Particle;
}

namespace adj {

class GraphNode;
typedef std::shared_ptr<GraphNode> GraphNodePtr;

// creates a node, and adds it to the particle system
class GraphNodeFactory {
public:
    static GraphNodeFactory& instance();
    static void cleanup();

    // parses a request from the network 
    void add_node_from_song_request(const Json::Value&);

    void add_empty_node();
    void add_to_random_node();
    void add_to_node(GraphNodePtr node);

    void delete_node(GraphNodePtr);

    std::vector<GraphNodePtr>& nodes() { return graph_nodes_; }

private:
    GraphNodeFactory();
    void init();

    static GraphNodeFactory* instance_;

    std::vector<GraphNodePtr> graph_nodes_;
};

}