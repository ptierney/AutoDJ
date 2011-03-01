
#include <assert.h>

#include <exception>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "cinder/Utilities.h"
#include "cinder/CinderMath.h"

#include "graph/graph_Particle.h"

#include "adj/adj_GraphNode.h"
#include "adj/adj_Song.h"
#include "adj/adj_CalloutBox.h"
#include "adj/adj_VoteManager.h"

namespace adj {

GraphNode::GraphNode() {
    is_valid_ = false;
    circle_radius_ = 5.0f;
    node_color_ = ci::ColorA(0.839f, 0.839f, 0.839f, 1.0f);
    node_highlight_color_ = ci::ColorA(0.0f, 0.643f, 1.0f, 1.0f);
    scale_ = 1.0f;
    max_scale_ = 1.0f;
    min_scale_ = 0.35f;
    is_current_song_ = false;
    is_next_song_ = false;
    is_transitioning_ = false;
    distance_from_current_ = 0;
    visible_ = false;
    fade_time_ = 1000; // in milliseconds
    display_time_ = 6000; // in milliseconds
    path_trigger_delay_time_ = 400; // in ms

    is_fading_in_ = false;
    is_fading_out_ = false;
    is_path_delaying_ = false;
}

void GraphNode::init() {
    // Graph nodes must have a song at the moment
    if (song_.get() == NULL)
        throw(std::runtime_error("Trying to create a graph node without a song"));
}

GraphNode::~GraphNode() {
    // remove from GraphNodeFactory list
    // remove / delete particle
    // remove / delete / stop song
}

void GraphNode::show() {
    if (visible_)
        return;

    // if there's no valid song, remain hidden
    if (song_.get() == NULL)
        return;

    // if it's not connected to the graph, remain hidden
    if (particle_.get() == NULL)
        return;

    if (callout_box_.get() == NULL) {
        callout_box_ = CalloutBoxPtr(new CalloutBox(*this));
        callout_box_->init();
        callout_box_->show();
    }

    update_appearance();

    visible_ = true;
}

void GraphNode::hide() {
    if (!visible_)
        return;

    visible_ = false;
}

void GraphNode::draw() {
    if (!visible_)
        return;

    // check if it's fading in / out, if it's been displayed long enough, etc
    check_transition_states();
    
    ci::gl::pushMatrices();

        ci::gl::translate(particle_->position());
            
        if (is_current_song_)
            draw_current_song();
        else
            draw_node();

    ci::gl::popMatrices();

    ci::gl::pushMatrices();

        callout_box_->draw();

    ci::gl::popMatrices();
}

void GraphNode::draw_current_song() {
    ci::gl::scale(ci::Vec3f::one() * max_scale_);
    ci::gl::color(node_highlight_color_);
    ci::gl::drawSolidCircle(ci::Vec2f::zero(), circle_radius_);
}

void GraphNode::draw_node() {
    // TODO: cache this value
    ci::gl::scale(ci::Vec3f::one() * ci::lmap<float>(distance_from_current_,
        0, 10, max_scale_, min_scale_));

    if (highlight_connection()) {
        ci::gl::color(node_highlight_color_);
        ci::gl::drawStrokedCircle(ci::Vec2f::zero(), circle_radius_);
    } else {
        ci::gl::color(node_color_);
        ci::gl::drawSolidCircle(ci::Vec2f::zero(), circle_radius_);
    }
}

void GraphNode::add_child(GraphNodePtr child) {
    // check children_ to see if it already contains child
    // TODO: replace this with a proper std::algorithm
    for (std::vector<GraphNodePtr>::const_iterator it = children_.begin();
        it != children_.end(); ++it) {
        if (it->get() == child.get())
            return;
    }

    children_.push_back(child);
}

void GraphNode::update_distance_from_current() {
    if (is_current_song_ || parent_.get() == NULL) {
        distance_from_current_ = 0;
        return;
    }

    int dist = 1;

    GraphNodePtr node = parent_;

    while (node->is_current_song() == false) {
        ++dist;
        node = node->parent();
    }

    distance_from_current_ = dist;
}

void GraphNode::set_is_transitioning(bool trans) {
    is_transitioning_ = trans;
    update_appearance();
}

void GraphNode::set_is_current_song(bool curr) {
    is_current_song_ = curr;
    update_appearance();
}

void GraphNode::set_is_next_song(bool next) {
    is_next_song_ = next;
    update_appearance();
}

void GraphNode::update_appearance() {
    if (!visible_)
        return;

    callout_box_->update_contents();
    update_distance_from_current();
}

void GraphNode::register_vote(VotePtr vote) {
    assert(vote->song_id == song_->id());

    song_->register_vote(vote);

    update_appearance();
}

void GraphNode::register_song_playing() {
    // begin transition
    start_callout_fadein();

    display_timer_ = boost::posix_time::microsec_clock::universal_time();

    is_path_delaying_ = true;
    path_trigger_timer_ = boost::posix_time::microsec_clock::universal_time();
}

// call this when it is added to the graph
void GraphNode::register_just_added() {
    start_callout_fadein();

    display_timer_ = boost::posix_time::microsec_clock::universal_time();

    is_path_delaying_ = true;
    path_trigger_timer_ = boost::posix_time::microsec_clock::universal_time();
}

void GraphNode::register_path_activate() {
    // begin transition
    start_callout_fadein();

    callout_box_->show();
    display_timer_ = boost::posix_time::microsec_clock::universal_time();

    // if you've reached the now playing (or transitioning) song, 
    // don't bother activating the path
    if (song_->is_playing())
        return;

    is_path_delaying_ = true;
    path_trigger_timer_ = boost::posix_time::microsec_clock::universal_time();
}

void GraphNode::start_callout_fadein() {
    is_fading_in_ = true;
    fade_timer_ = boost::posix_time::microsec_clock::universal_time();
}

void GraphNode::start_callout_fadeout() {
    is_fading_out_ = true;
    fade_timer_ = boost::posix_time::microsec_clock::universal_time();
}

int GraphNode::get_milliseconds_elapsed(boost::posix_time::ptime& t) {
    boost::posix_time::time_duration diff = 
        boost::posix_time::microsec_clock::universal_time() - t;

    return diff.total_milliseconds();
}

void GraphNode::check_transition_states() {
    if (is_fading_in_)
        transition_fading_in();

    if (is_fading_out_)
        transition_fading_out();

    if (callout_box_->visible())
        check_display_time();

    if (is_path_delaying_)
        check_path_delay();
}

void GraphNode::transition_fading_in() {
    int elapsed = get_milliseconds_elapsed(fade_timer_);

    if (elapsed > fade_time_) {
        is_fading_in_ = false;
        callout_box_->set_alpha(1.0f);
        return;
    }

    callout_box_->set_alpha(ci::lmap<float>(elapsed, 0, fade_time_, 0.0f, 1.0f));
}

void GraphNode::transition_fading_out() {
    int elapsed = get_milliseconds_elapsed(fade_timer_);

    if (elapsed > fade_time_) {
        is_fading_out_ = false;
        callout_box_->hide();
        return;
    }

    callout_box_->set_alpha(ci::lmap<float>(elapsed, 0, fade_time_, 1.0f, 0.0f));
}

void GraphNode::check_display_time() {
    int elapsed = get_milliseconds_elapsed(display_timer_);

    if (elapsed < display_time_)
        return;

    if (is_fading_out_)
        return;

    start_callout_fadeout();
}

void GraphNode::check_path_delay() {
    int elapsed = get_milliseconds_elapsed(display_timer_);

    if (elapsed < path_trigger_delay_time_)
        return;

    is_path_delaying_ = false;

    trigger_next_node();
}

void GraphNode::trigger_next_node() {
    if (parent_.get() == NULL)
        return;

    parent_->register_path_activate();
}

bool GraphNode::highlight_connection() {
    return callout_box_->visible();
}

}
