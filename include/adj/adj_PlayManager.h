
#pragma once

#include "boost/date_time/posix_time/posix_time.hpp"

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

    GraphNodePtr now_playing() { return now_playing_; }
    void set_now_playing(GraphNodePtr);

    void play();
    void pause();
    void next_song(); // begins transitioning to next song

    // should be private
    void switch_to_next_song();

    friend class GraphNodeFactory;

private:
    PlayManager();
    void init();
    void begin_transition();
    GraphNodePtr get_next_song_randomly();
    GraphNodePtr get_next_song(); //TODO: implement this method, doesn't work.
    
    void begin_override_transition();
    int override_elapsed();

    void register_new_graph_node(GraphNodePtr);

    GraphNodePtr now_playing_;
    GraphNodePtr next_song_; // for transitions

    int transition_time_; // time for transition overlap
    bool transitioning_;

    bool override_transitioning_;
    boost::posix_time::ptime override_timer_;

    static PlayManager* instance_;

};

}