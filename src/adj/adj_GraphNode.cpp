
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Utilities.h"

#include "graph/graph_Particle.h"

#include "adj/adj_GraphNode.h"
#include "adj/adj_Song.h"

namespace adj {

GraphNode::GraphNode() {
    is_valid_ = false;
    size_ = 7.0f;
    node_color_ = ci::ColorA(1.0f, 1.0f, 1.0f, 0.9f);
    text_color_ = ci::ColorA(1.0f, 0.0f, 1.0f, 1.0f);
    text_font_ = "/data/isocpeur.ttf";
    text_font_i_ = "/data/isocpeui.ttf";
    scale_ = 1.0f;
    font_size_ = 64;
}

void GraphNode::init() {
    if (song_.get() == NULL) // wtf
        return;

    // create a text texture
    refresh_text();
}

void GraphNode::draw() {
    ci::gl::color(node_color_);

    ci::gl::pushMatrices();

        ci::gl::translate(particle_->position());
        ci::gl::scale(ci::Vec3f::one() * scale_);

        ci::gl::drawSolidCircle(ci::Vec2f::zero(), size_);

        ci::gl::scale(ci::Vec3f::one() * 0.08f);

        ci::gl::translate(-text_texture_.getSize() / 2.0f);

        ci::gl::color(ci::Color::white());

        ci::gl::draw(text_texture_, ci::Vec2f::zero());

    ci::gl::popMatrices();
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


// create a new text texture
void GraphNode::refresh_text() {
    ci::TextLayout layout;
    layout.setFont(ci::Font(text_font_, font_size_));
    layout.setColor(text_color_);
    layout.addCenteredLine(song_->name());
    layout.setFont(ci::Font(text_font_i_, font_size_));
    layout.addCenteredLine(song_->artist());
    layout.addCenteredLine(song_->album());
    // this can be premultiplied whatever that means
    ci::Surface8u rendered = layout.render(true);
    // this might want to go in another thread
    text_texture_ = ci::gl::Texture(rendered);
}

}
