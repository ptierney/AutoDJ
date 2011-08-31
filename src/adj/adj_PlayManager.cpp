
#include "boost/date_time/posix_time/posix_time.hpp"

#include <adj/adj_Adj.h>

#include "cinder/app/App.h"
#include "cinder/Rand.h"

#include "adj/adj_PlayManager.h"
#include "adj/adj_GraphNode.h"
#include "adj/adj_Song.h"

namespace adj {

PlayManager* PlayManager::instance_ = NULL;

PlayManager::PlayManager() {
    transition_time_ = 5; // time in seconds
    transitioning_ = false;
    override_transitioning_ = false;
}

void PlayManager::init() {
    now_playing_ = GraphNodePtr();
    next_song_ = GraphNodePtr();
}

void PlayManager::setup() {
    // nothing here
}

void PlayManager::update() {
    // at the moment, with the Crowdtap configuration, this isn't needed
    return;

    if (now_playing_.get() == NULL)
        return;

    if (override_transitioning_ && (override_elapsed() >= transition_time_)) {
        switch_to_next_song();
        return;
    }

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

void PlayManager::play() {
    if (now_playing_->song().is_playing())
        return;

    now_playing_->song().play();
}

void PlayManager::pause() {
    if (now_playing_->song().is_playing() == false)
        return;

    now_playing_->song().pause();
}

void PlayManager::next_song() {
    begin_override_transition();
    
}

void PlayManager::begin_transition() {
    // if the next song request hasn't got back in time, just
    // choose a song at random
    if (next_song_.get() == NULL) {
        next_song_ = get_next_song_randomly();
        next_song_->set_is_next_song(true);
    }

    transitioning_ = true;
    next_song_->set_is_transitioning(true);

    next_song_->song().play();
    
}

void PlayManager::begin_override_transition() {
    override_transitioning_ = true;

    override_timer_ = SongFactory::instance().get_current_time();

    begin_transition();
}

int PlayManager::override_elapsed() {
    boost::posix_time::time_duration diff = 
        SongFactory::instance().get_current_time() - override_timer_;

    return diff.total_seconds();
}

void PlayManager::switch_to_next_song() {
    if (next_song_.get() == NULL) {
        next_song_ = get_next_song_randomly();
        next_song_->set_is_next_song(true);
    }

    // clean up now playing
    now_playing_->song().stop();
    now_playing_->set_is_current_song(false);
    now_playing_->set_is_transitioning(false);

    override_transitioning_ = false;
    transitioning_ = false;

    next_song_->set_is_transitioning(false);
    next_song_->set_is_next_song(false);
    next_song_->set_is_current_song(true);

    now_playing_ = next_song_;
    next_song_.reset();

    now_playing_->song().play(); // if it wasn't already

    next_song_ = get_next_song_randomly(); // this should be non-randomly

    if (next_song_.get() != NULL)
        next_song_->set_is_next_song(true);
    else
        ci::app::console() << "**WARNING** Unable to get a next song" << std::endl;
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

// NOTE: this method should nat assume that the node's particle
// is set up correctly, only the song
void PlayManager::register_new_graph_node(GraphNodePtr node) {
    if (now_playing_.get() != NULL)
        return;

    node->set_is_current_song(true);
    node->set_is_next_song(false);
    node->set_is_transitioning(false);

    now_playing_ = node;
    now_playing_->song().play();
}

void PlayManager::set_now_playing(GraphNodePtr np) {
    now_playing_ = np;
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
