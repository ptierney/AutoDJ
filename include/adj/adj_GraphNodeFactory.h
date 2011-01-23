
#pragma once

#include "adj/adj_Adj.h"

#include <vector>
#include <map>
#include <utility>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"

#include "adj/adj_Song.h"

namespace graph {
    class ParticleSystem;
    class Particle;
}

namespace adj {

class GraphNode;
typedef std::shared_ptr<GraphNode> GraphNodePtr;
typedef std::shared_ptr<struct Vote> VotePtr;


// the graph oracle uses these to communicate new song requests
// requests that a node with a SongId be joined to another song
typedef std::pair<SongId, SongId> PairRequest;

// creates a node, and adds it to the particle system
class GraphNodeFactory {
public:
    static GraphNodeFactory& instance();
    static void cleanup();

    // parses a request from the network 
    void update_graph_from_vote(VotePtr);

    // these are primarily debugging methods. All actual node creation
    // is done through votes
    void add_empty_node();
    void add_new_to_random_node();
    void add_new_to_node(GraphNodePtr node);

    void delete_node(GraphNodePtr);

    std::vector<GraphNodePtr>& nodes() { return graph_nodes_; }
    const std::map<SongId, GraphNodePtr>& song_map() { return song_map_; }

    void add_pair_request(PairRequest);

    void update();

private:
    GraphNodeFactory();
    void init();
    void create_new_node_from_vote(VotePtr);
    GraphNodePtr create_new_node(SongId);
    void add_node_to_physics(GraphNodePtr);
    // attatch a to b
    void connect_nodes(GraphNodePtr a, GraphNodePtr b);
    void check_pair_requests();
    void pair_nodes(SongId, SongId);
    void assert_node_has_particle(GraphNodePtr);

    static GraphNodeFactory* instance_;

    // vector is used for iterating through to draw
    std::vector<GraphNodePtr> graph_nodes_;
    // map is used for searching
    std::map<SongId, GraphNodePtr> song_map_;

    std::map<SongId, GraphNodePtr>::iterator song_map_it_;

    std::deque<PairRequest> pair_requests_;
    boost::mutex pair_requests_mutex_;
};

}
