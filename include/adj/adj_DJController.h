
#pragma once

// This is a Crowtap specific function
// It is used by the DJ to control the visualizer

#include <adj/adj_Song.h>

namespace adj {

class DJController {
public:
    DJController();
    void init();

    void set_next_song(SongId);
    void transition();

    static DJController& instance();

private:
    void set_next_song_randomly();

    bool has_next_song_;
    SongId next_song_;

    static DJController* instance_;
};

}
