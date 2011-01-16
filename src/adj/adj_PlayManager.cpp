
#include "adj/adj_PlayManager.h"

namespace adj {

PlayManager* PlayManager::instance_ = NULL;

PlayManager::PlayManager() {
    transition_time_ = 10; // time in seconds
    transitioning_ = false;
}

void PlayManager::init() {
    now_playing_ = GraphNodePtr();
    next_song_ = GraphNodePtr();
}

void PlayManager::setup() {
    // nothing here
}

void PlayManager::update() {
    if (now_playing_.get() == NULL)
        return;

    // if time remaining on now_playing > 10 seconds
    // return

    if (!transitioning_) {
        begin_transition();
        return;
    }

    // if 

}

void PlayManager::begin_transition() {
    // if the next song request hasn't got back in time, just
    // choose a song at random
    if (next_song_.get() == NULL)
        next_song_ = get_next_song_randomly();

    transitioning_ = true;

    play_song(next_song_);
}

// TODO: use amazing algorithm to figure out next song
GraphNodePtr PlayManager::get_next_song() {
    return GraphNodePtr();
}

PlayManager& PlayManager::instance() {
    if (instance_ == NULL) {
        instance_ = new PlayManager();
        instance_->init();
    }

    return *instance_;
}

void PlayManager::cleanup() {
    if (instance_ == NULL)
        return;

    delete instance_;
}

}