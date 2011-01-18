
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/Surface.h"

#include "adj/adj_CalloutBox.h"
#include "adj/adj_GraphNode.h"
#include "adj/adj_Song.h"

namespace adj {

CalloutBox::CalloutBox(GraphNode& parent) : node_(parent) {
    visible_ = true;
    text_color_ = ci::ColorA(1.0f, 0.0f, 1.0f, 1.0f);
    text_font_ = "/data/isocpeur.ttf";
    text_font_i_ = "/data/isocpeui.ttf";
    font_size_ = 64;
}

void CalloutBox::init() {
    update_contents();
}

void CalloutBox::update_contents() {
    // check the state of the GraphNode
    // built up a Cairo Context / texture based on this state
    // render context to opengl texture

    // temporary:
    refresh_text();
}

void CalloutBox::draw() {
    if (visible_ == false)
        return;

    ci::gl::scale(ci::Vec3f::one() * 0.08f);

    ci::gl::translate(-text_texture_.getSize() / 2.0f);

    ci::gl::color(ci::Color::white());

    ci::gl::draw(text_texture_, ci::Vec2f::zero());

    // render prerendered cairo surface
}

void CalloutBox::show() {
    visible_ = true;
    update_contents();
}

void CalloutBox::hide() {
    visible_ = false;
}

void CalloutBox::set_contents_current() {
    // TODO: make
}

void CalloutBox::set_contents_node() {

}

void CalloutBox::set_contents_just_added() {
    // TODO: make
}

// create a new text texture
void CalloutBox::refresh_text() {
    ci::TextLayout layout;
    layout.setFont(ci::Font(text_font_, static_cast<float>(font_size_)));
    layout.setColor(text_color_);
    layout.addCenteredLine(node_.song().name());
    layout.setFont(ci::Font(text_font_i_, static_cast<float>(font_size_)));
    layout.addCenteredLine(node_.song().artist());
    layout.addCenteredLine(node_.song().album());
    // this can be premultiplied whatever that means
    ci::Surface8u rendered = layout.render(true);
    // this might want to go in another thread
    text_texture_ = ci::gl::Texture(rendered);
}

}

