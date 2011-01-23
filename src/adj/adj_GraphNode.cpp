
#include <assert.h>

#include <exception>

#include "cinder/gl/gl.h"
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

void GraphNode::draw_transitioning_out() {

}

void GraphNode::draw_transitioning_in() {

}

void GraphNode::draw_node() {
    // TODO: cache this value
    ci::gl::scale(ci::Vec3f::one() * ci::lmap<float>(distance_from_current_,
        0, 10, max_scale_, min_scale_));
    ci::gl::color(node_color_);
    ci::gl::drawSolidCircle(ci::Vec2f::zero(), circle_radius_);
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
    callout_box_->update_contents();
}

void GraphNode::set_is_current_song(bool curr) {
    is_current_song_ = curr;
    callout_box_->update_contents();
}

void GraphNode::set_is_next_song(bool next) {
    is_next_song_ = next;
    callout_box_->update_contents();
}

void GraphNode::update_appearance() {
    // maybe change size
    // for sure update the callout box to include any new votes
    update_distance_from_current();
}

void GraphNode::register_vote(VotePtr vote) {
    assert(vote->song_id == song_->id());

    song_->register_vote(vote);

    update_appearance();
}


}
