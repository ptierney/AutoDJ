
#pragma once

#include "adj/adj_Adj.h"

#include <vector>
#include <map>

#include "adj/adj_Song.h"

namespace graph {
    class ParticleSystem;
    class Particle;
}

namespace adj {

class GraphNode;
typedef std::shared_ptr<GraphNode> GraphNodePtr;
typedef std::shared_ptr<struct Vote> VotePtr;

// creates a node, and adds it to the particle system
class GraphNodeFactory {
public:
    static GraphNodeFactory& instance();
    static void cleanup();

    // parses a request from the network 
    void update_graph_from_vote(VotePtr);

    void add_empty_node();
    void add_to_random_node();
    void add_to_node(GraphNodePtr node);

    void delete_node(GraphNodePtr);

    const std::vector<GraphNodePtr>& nodes() { return graph_nodes_; }
    const std::map<SongId, GraphNodePtr>& song_map() { return song_map_; }

private:
    GraphNodeFactory();
    void init();
    void create_new_node_from_vote(VotePtr);

    static GraphNodeFactory* instance_;

    // vector is used for iterating through to draw
    std::vector<GraphNodePtr> graph_nodes_;
    // map is used for searching
    std::map<SongId, GraphNodePtr> song_map_;

    std::map<SongId, GraphNodePtr>::iterator song_map_it_;
};

}
