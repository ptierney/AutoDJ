
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

#include "graph/graph_Particle.h"

#include "adj/adj_CalloutBox.h"
#include "adj/adj_GraphNode.h"
#include "adj/adj_Song.h"
#include "Resources.h"
#include "adj/adj_GraphPhysics.h"

namespace adj {

CalloutBox::CalloutBox(GraphNode& parent) : node_(parent) {
    visible_ = true;
    text_color_ = ci::ColorA(1.0f, 0.0f, 1.0f, 1.0f);
    font_size_ = 48;
    
    side_margin_ = 20;
    top_margin_ = 20;
    text_spacing_ = 10;

    line_width_ = 8.0f;
    dash_gap_size_ = 2 * line_width_; // remember this accounts for circle size

    scale_ = 0.08f;
}

void CalloutBox::init() {
    medium_font_ = ci::Font(ci::app::loadResource(RES_MEDIUM_FONT), font_size_);

    particle_ = GraphPhysics::instance().create_box_particle(node_.particle());

    update_contents();
}

void CalloutBox::update_contents() {
    calculate_surface_size();
    create_surface();
    create_context();
    update_box_position();

    set_contents_node();
}

void CalloutBox::calculate_surface_size() {
    surface_size_ = ci::Vec2i(640, 300);
}

void CalloutBox::create_surface() {
    surface_ = std::shared_ptr<ci::cairo::SurfaceImage>(new 
        ci::cairo::SurfaceImage(surface_size_.x, surface_size_.y, true));
}

void CalloutBox::create_context() {
    context_ = std::shared_ptr<ci::cairo::Context>(new
        ci::cairo::Context(*(surface_.get())));
}

void CalloutBox::create_connect_surface() {

}

void CalloutBox::create_connect_context() {

}

void CalloutBox::update_box_position() {
    //box_position_ = ci::Vec2f(-surface_size_.x, -surface_size_.y / 2.0f);
    // make the surface centered on the 
    box_position_ = particle_->position();
}

void CalloutBox::draw() {
    if (visible_ == false)
        return;

    update_box_position();

    ci::gl::translate(box_position_);
    ci::gl::scale(ci::Vec3f::one() * scale_);

    ci::gl::color(ci::Color::white());

    ci::gl::draw(text_texture_, ci::Vec2f::zero());

    draw_connection();
    // render prerendered cairo surface
}

void CalloutBox::draw_connection() {
    create_connect_surface();
    create_connect_context();
    render_connection();
}

void CalloutBox::render_connection() {
    // build up the lines in cairo
    // render with OpenGL
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

    context_->setFont(medium_font_);
    context_->setFontSize(static_cast<double>(font_size_));
    context_->setSourceRgb(1.0f, 1.0f, 1.0f);
    context_->moveTo(side_margin_, top_margin_ + font_size_);
    context_->showText(boost::lexical_cast<std::string>(node_.distance_from_current()));
    context_->moveTo(side_margin_, top_margin_ + font_size_ + 
        (font_size_ + text_spacing_));
    context_->setFont(medium_font_);    context_->showText(node_.song().name());
    context_->moveTo(side_margin_, top_margin_ + font_size_ + 
        2 * (font_size_ + text_spacing_));
    context_->showText(node_.song().artist());

    context_->setSourceRgb(node_.node_highlight_color().r, node_.node_highlight_color().g,
        node_.node_highlight_color().b);

    //context_setup_dash();
    context_setup_solid();

    context_->newPath();
    context_->moveTo(line_width_, line_width_);
    context_->lineTo(surface_size_.x - line_width_, line_width_);
    context_->lineTo(surface_size_.x - line_width_, surface_size_.y - line_width_);
    context_->lineTo(line_width_, surface_size_.y - line_width_);
    context_->closePath();
    context_->stroke();

    /*
    context_->line(ci::Vec2f(surface_size_.x / 2, 0 + line_width_), 
        ci::Vec2f(surface_size_.x, surface_size_.y / 2));
    context_->line(ci::Vec2f(surface_size_.x / 2, surface_size_.y - line_width_), 
        ci::Vec2f(surface_size_.x, surface_size_.y / 2));

    context_->stroke();
    */
    
    text_texture_ = ci::gl::Texture(surface_->getSurface());
}

void CalloutBox::set_contents_just_added() {
    // TODO: make
}

// create a new text texture
void CalloutBox::refresh_text() {
    ci::TextLayout layout;
    layout.setFont(ci::Font(ci::app::loadResource(RES_MEDIUM_FONT), 
        static_cast<float>(font_size_)));
    layout.setColor(text_color_);
    layout.addCenteredLine(node_.song().name());
    layout.addCenteredLine(node_.song().artist());
    layout.addCenteredLine(node_.song().album());
    // this can be premultiplied whatever that means
    ci::Surface8u rendered = layout.render(true);
    // this might want to go in another thread
    text_texture_ = ci::gl::Texture(rendered);
}

void CalloutBox::context_setup_dash() {
    // scale_ division needed to transform from parent coordinates
    context_->setLineCap(ci::cairo::LINE_CAP_ROUND);
    context_->setLineWidth(line_width_);

    std::vector<double> dash;
    dash.push_back(0.0); // dash size
    dash.push_back(line_width_ * 2); // gap size

    context_->setDash(dash);
}

void CalloutBox::context_setup_solid() {
    context_->setLineCap(ci::cairo::LINE_CAP_ROUND);
    context_->setLineWidth(line_width_);
}

}

