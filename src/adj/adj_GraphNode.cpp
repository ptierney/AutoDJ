
#include <assert.h>

#include <exception>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <cinder/gl/gl.h>
#include <cinder/app/App.h>
#include <cinder/Utilities.h>
#include <cinder/CinderMath.h>
#include <cinder/Rand.h>

#include <graph/graph_Particle.h>
#include <graph/graph_Spring.h>

#include <AdjApp.h>
#include <adj/adj_GraphNode.h>
#include <adj/adj_Song.h>
#include <adj/adj_CalloutBox.h>
#include <adj/adj_VoteManager.h>
#include <adj/adj_GraphPhysics.h>
#include <adj/adj_Renderer.h>

#include <Resources.h>

namespace adj {

GraphNode::GraphNode() {
    is_valid_ = false;
    circle_radius_ = 5.0f;
    inner_circle_radius_ = 2.0f;
    num_segments_ = 30; // this controls the "resolution" of the circles



    //node_highlight_color_ = Renderer::instance().highlight_color();
    //node_highlight_color_ = Renderer::instance().color_palette()[4];

    background_color_ = Renderer::instance().background_color();

    scale_ = 1.0f;
    max_scale_ = 1.0f;
    min_scale_ = 0.35f;
    is_current_song_ = false;
    is_next_song_ = false;
    is_transitioning_ = false;
    distance_from_current_ = 0;
    visible_ = false;
    fade_time_ = 2000; // in milliseconds
    display_time_ = 25000; // in milliseconds
    path_trigger_delay_time_ = 400; // in ms

    relax_time_ = 600000; // in milliseconds
    closest_spring_length_ = 20.0f;
    closest_spring_strength_ = 0.2f;
    farthest_spring_length_ = 70.0f;
    farthest_spring_strength_ = 0.1f;

    is_fading_in_ = false;
    is_fading_out_ = false;
    is_path_delaying_ = false;
}

void GraphNode::init() {
    // Graph nodes must have a song at the moment
    if (song_.get() == NULL)
        throw(std::runtime_error("Trying to create a graph node without a song"));

    last_vote_time_ = boost::posix_time::microsec_clock::universal_time();
    
    ci::Rand rand;
    rand.seed(song_->id());

    circle_radius_ = rand.randFloat(4.5f, 5.5f);
    inner_circle_radius_ = rand.randFloat(1.5f, 3.5f);

    node_color_ = Renderer::instance().color_palette()[rand.randFloat(1, 4)];
    node_highlight_color_ = node_color_;
}

GraphNode::~GraphNode() {
    // remove / delete particle
    GraphPhysics::instance().remove_particle(particle_);

    // delete callout box
    callout_box_.reset();

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

float GraphNode::get_width() {
    return 2.0f * circle_radius_;
}

float GraphNode::get_height() {
    return 2.0f * circle_radius_;
}

float GraphNode::get_pos_x() {
    return particle_->position().x;
}

float GraphNode::get_pos_y() {
    return particle_->position().y;
}

void GraphNode::draw() {
    if (!visible_)
        return;

    update_spring_values();

    // check if it's fading in / out, if it's been displayed long enough, etc
    check_transition_states();
}

void GraphNode::draw_callout_box() {
    ci::gl::pushMatrices();

        callout_box_->draw();

    ci::gl::popMatrices();
}

void GraphNode::draw_node_body() {
    ci::gl::pushMatrices();

        ci::gl::translate(particle_->position());
            
        if (is_current_song_)
            draw_current_song();
        else
            draw_node();

    ci::gl::popMatrices();
}

void GraphNode::draw_current_song() {
    ci::gl::scale(ci::Vec3f::one() * max_scale_);
    ci::gl::color(node_highlight_color_);
    ci::gl::drawSolidCircle(ci::Vec2f::zero(), circle_radius_);
}

void GraphNode::draw_node() {
    if (highlight_connection()) {
        ci::gl::color(node_highlight_color_);
        ci::gl::drawSolidCircle(ci::Vec2f::zero(), circle_radius_, num_segments_);
        
        ci::gl::color(background_color_);
        ci::gl::drawSolidCircle(ci::Vec2f::zero(), inner_circle_radius_, 
            num_segments_);        
    } else {
        ci::gl::color(node_color_);
        ci::gl::drawSolidCircle(ci::Vec2f::zero(), circle_radius_, num_segments_);
        
        ci::gl::color(background_color_);
        ci::gl::drawSolidCircle(ci::Vec2f::zero(), inner_circle_radius_,
            num_segments_);
    }
}

void GraphNode::add_child(GraphNodePtr child) {
    std::vector<GraphNodePtr>::iterator it = std::find(
        children_.begin(), children_.end(), child);

    if (it != children_.end())
        return;

    children_.push_back(child);
}

void GraphNode::remove_child(GraphNodePtr child) {
    std::vector<GraphNodePtr>::iterator it = std::find(
        children_.begin(), children_.end(), child);

    if (it == children_.end())
        return;

    children_.erase(it);
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

    last_vote_time_ = boost::posix_time::microsec_clock::universal_time();

    if (callout_box_.get() != NULL) {
        callout_box_->show();

        start_callout_fadein();

        display_timer_ = boost::posix_time::microsec_clock::universal_time();
    }

    update_appearance();
}

void GraphNode::register_song_playing() {
    node_highlight_color_ = Renderer::instance().color_palette()[4];

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

    //is_path_delaying_ = true;
    //path_trigger_timer_ = boost::posix_time::microsec_clock::universal_time();
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

    if (is_fading_out_ && !is_current_song_)
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

void GraphNode::update_spring_values() {
    if (particle_->springs().empty())
        return;

    int elapsed = get_milliseconds_elapsed(last_vote_time_);

    float strength = ci::lmap<float>(elapsed, 0, relax_time_,
        closest_spring_strength_, farthest_spring_strength_);

    strength = ci::math<float>::clamp(strength, closest_spring_strength_,
        farthest_spring_strength_);

    float length = ci::lmap<float>(elapsed, 0, relax_time_, 
        closest_spring_length_, farthest_spring_length_);

    length = ci::math<float>::clamp(length, closest_spring_length_,
        farthest_spring_length_);

    particle_->springs()[0]->set_rest_length(length);
    particle_->springs()[0]->set_strength(strength);
}

}
