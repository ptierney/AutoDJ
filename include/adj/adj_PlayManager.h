
#pragma once

#include "adj/adj_Adj.h"
#include "adj/adj_GraphNodeFactory.h"

namespace adj {

class GraphNode;
typedef std::shared_ptr<GraphNode> GraphNodePtr;

class PlayManager {
public:
    static PlayManager& instance();
    static void cleanup();

    void setup();
    void update();

    GraphNodePtr now_playing();
    void set_now_playing(GraphNodePtr);

    void play();
    void pause();

    friend class GraphNodeFactory;

private:
    PlayManager();
    void init();
    void begin_transition();
    GraphNodePtr get_next_song_randomly();
    GraphNodePtr get_next_song(); //TODO: implement this method, doesn't work.
    void switch_to_next_song();

    void register_new_graph_node(GraphNodePtr);

    GraphNodePtr now_playing_;
    GraphNodePtr next_song_; // for transitions

    int transition_time_; // time for transition overlap
    bool transitioning_;

    static PlayManager* instance_;

};

}