
#include "boost/lexical_cast.hpp"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Font.h"
#include "cinder/Utilities.h"
#include "cinder/Surface.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/ImageIo.h"
#include "cinder/app/AppNative.h"

#include "adj/adj_CalloutBox.h"
#include "adj/adj_GraphNode.h"
#include "adj/adj_Song.h"
#include "Resources.h"

namespace adj {

CalloutBox::CalloutBox(GraphNode& parent) : node_(parent) {
    visible_ = true;
    text_color_ = ci::ColorA(1.0f, 0.0f, 1.0f, 1.0f);
    text_font_ = "/data/isocpeur.ttf";
    text_font_i_ = "/data/isocpeui.ttf";
    font_size_ = 48;
    surface_size_ = ci::Vec2i(640, 300);

    side_margin_ = 20;
    top_margin_ = 20;
    text_spacing_ = 10;

    line_width_ = 8.0f;

    scale_ = 0.08f;
}

void CalloutBox::init() {
    medium_font_ = ci::Font(ci::app::loadResource(RES_MEDIUM_FONT), font_size_);
    italic_font_ = ci::Font(ci::app::loadResource(RES_ITALIC_FONT), font_size_);
    bold_font_ = ci::Font(ci::app::loadResource(RES_BOLD_FONT), font_size_);

    surface_ = std::shared_ptr<ci::cairo::SurfaceImage>(new 
        ci::cairo::SurfaceImage(surface_size_.x, surface_size_.y, true));
    
    context_ = std::shared_ptr<ci::cairo::Context>(new
        ci::cairo::Context(*(surface_.get())));

    update_contents();
}

void CalloutBox::update_contents() {
    // check the state of the GraphNode
    // built up a Cairo Context / texture based on this state
    // render context to opengl texture
    set_contents_node();

    // temporary:
    //refresh_text();
}

void CalloutBox::draw() {
    if (visible_ == false)
        return;

    ci::gl::scale(ci::Vec3f::one() * scale_);
    ci::gl::translate(ci::Vec2f(-surface_size_.x, -surface_size_.y / 2.0f));

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

    context_->setFont(italic_font_);
    context_->setFontSize(static_cast<double>(font_size_));
    context_->setSourceRgb(1.0f, 1.0f, 1.0f);
    context_->moveTo(side_margin_, top_margin_ + font_size_);
    context_->showText(boost::lexical_cast<std::string>(node_.distance_from_current()));
    context_->moveTo(side_margin_, top_margin_ + font_size_ + 
        (font_size_ + text_spacing_));
    context_->setFont(medium_font_);
    context_->showText(node_.song().name());
    context_->moveTo(side_margin_, top_margin_ + font_size_ + 
        2 * (font_size_ + text_spacing_));
    context_->showText(node_.song().artist());

    context_->setSourceRgb(node_.node_highlight_color().r, node_.node_highlight_color().g,
        node_.node_highlight_color().b);

    /*
    context_->setSourceRgb(1.0, 1.0, 1.0);
    context_->setLineWidth(4.0);
    context_->moveTo(ci::Vec2f(surface_size_.x / 2, 0));
    context_->lineTo(ci::Vec2f(surface_size_.x, surface_size_.y / 2));
    */

    // scale_ division needed to transform from parent coordinates
    context_->setLineCap(ci::cairo::LINE_CAP_ROUND);
    context_->setLineWidth(line_width_);

    std::vector<double> dash;
    dash.push_back(0.0);
    dash.push_back(line_width_ * 2);

    context_->setDash(dash);
    context_->line(ci::Vec2f(surface_size_.x / 2, 0 + line_width_), 
        ci::Vec2f(surface_size_.x, surface_size_.y / 2));
    context_->line(ci::Vec2f(surface_size_.x / 2, surface_size_.y - line_width_), 
        ci::Vec2f(surface_size_.x, surface_size_.y / 2));

    context_->stroke();
    
    text_texture_ = ci::gl::Texture(surface_->getSurface());
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

