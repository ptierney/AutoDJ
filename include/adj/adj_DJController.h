
#pragma once

// This is a Crowtap specific function
// It is used by the DJ to control the visualizer

#include <adj/adj_Song.h>

namespace adj {

class DJController {
public:
    DJController();
    void init();
    
    void update();

    void set_next_song(SongId);
    void transition();

    void set_transition_next_loop(bool t) {	
        transition_next_loop_ = t; }
    
    bool transition_next_loop() { return transition_next_loop_; }
    
    static DJController& instance();

private:
    void set_next_song_randomly();

    bool has_next_song_;
    SongId next_song_;
    
    bool transition_next_loop_;
    int counter_;

    static DJController* instance_;
};

}
