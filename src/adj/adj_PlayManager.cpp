
#include "cinder/Rand.h"

#include "adj/adj_PlayManager.h"
#include "adj/adj_GraphNode.h"
#include "adj/adj_Song.h"

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

    if (now_playing_->song().time_remaining() > transition_time_)
        return;

    if (!transitioning_) {
        begin_transition();
        return;
    } else {
        if (now_playing_->song().time_remaining() <= 0) {
            switch_to_next_song();
        }
    }
}

void PlayManager::begin_transition() {
    // if the next song request hasn't got back in time, just
    // choose a song at random
    if (next_song_.get() == NULL)
        next_song_ = get_next_song_randomly();

    transitioning_ = true;

    next_song_->song().play();
}

void PlayManager::switch_to_next_song() {
    if (next_song_.get() == NULL)
        next_song_ = get_next_song_randomly();

    now_playing_ = next_song_;
    next_song_.reset();

    get_next_song_randomly(); // this should be non-randomly
}

// TODO: use amazing algorithm to figure out next song
GraphNodePtr PlayManager::get_next_song() {
    return GraphNodePtr();
}

GraphNodePtr PlayManager::get_next_song_randomly() {
    if (now_playing_->children().empty())
        return GraphNodePtr();

    ci::Rand rand;
    rand.randomize();

    return now_playing_->children()[rand.randInt(
        now_playing_->children().size() - 1)];
}

void PlayManager::register_new_graph_node(GraphNodePtr node) {
    if (now_playing_.get() != NULL)
        return;

    now_playing_ = node;
    now_playing_->song().play();
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