
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
#include "cinder/CinderMath.h"

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
    
    is_left_of_node_ = false;
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
    surface_size_ = ci::Vec2f(640, 300);
}

void CalloutBox::create_surface() {
    surface_ = std::shared_ptr<ci::cairo::SurfaceImage>(new 
        ci::cairo::SurfaceImage(surface_size_.x, surface_size_.y, true));
}

void CalloutBox::create_context() {
    context_ = std::shared_ptr<ci::cairo::Context>(new
        ci::cairo::Context(*(surface_.get())));
}



void CalloutBox::update_box_position() {
    //box_position_ = ci::Vec2f(-surface_size_.x, -surface_size_.y / 2.0f);
    // make the surface centered on the 
    box_position_ = particle_->position();
    box_top_y_= box_position_.y - surface_size_.y / 2.0f * scale_;
    box_bottom_y_ = box_position_.y + surface_size_.y / 2.0f * scale_;
}

void CalloutBox::draw() {
    if (visible_ == false)
        return;

    update_box_position();

    ci::gl::translate(box_position_);
    ci::gl::scale(ci::Vec3f::one() * scale_);

    ci::gl::color(ci::Color::white());

    draw_connection();

    // translate correctly
    if (is_left_of_node_)
        ci::gl::draw(text_texture_, ci::Vec2f(-surface_size_.x, 
        -surface_size_.y / 2.0f));
    else
        ci::gl::draw(text_texture_, ci::Vec2f(0.0f, -surface_size_.y / 2.0f));
}

void CalloutBox::draw_connection() {
    render_connection();
}

void CalloutBox::create_connect_surface() {
}

void CalloutBox::create_connect_context() {
}

void CalloutBox::render_connection() {
    is_next_to_node_ = is_left_of_node_ = is_below_node_ = false;

    // build up the lines in cairo
    // render with OpenGL
    if (particle_->position().x < node_.particle()->position().x)
        is_left_of_node_ = true;

    if (particle_->position().y > node_.particle()->position().y)
        is_below_node_ = true;

    if (node_.particle()->position().y > 
        particle_->position().y - surface_size_.y / 2.0f * scale_ &&
        node_.particle()->position().y <
        particle_->position().y + surface_size_.y / 2.0f * scale_)
        is_next_to_node_ = true;

    float box_top_offset_, box_bottom_offset_, node_top_offset_, node_bottom_offset_;
    ci::Vec2f node_pos = node_.particle()->position();

    if (is_next_to_node_) {
        box_top_offset_ = box_bottom_offset_ = 0.0f;
        node_top_offset_ = node_pos.y - box_top_y_;
        node_bottom_offset_ = box_bottom_y_ - node_pos.y;
    } else {
        if (is_below_node_) {
            box_top_offset_ = box_top_y_ - node_pos.y;
            box_bottom_offset_ = 0.0f;
            node_bottom_offset_ = box_bottom_y_ - node_pos.y;
            node_top_offset_ = 0.0f;
        } else { // is on top of node
            box_top_offset_ = 0.0f;
            box_bottom_offset_ = node_pos.y - box_bottom_y_;
            node_top_offset_ = node_pos.y - box_top_y_;
            node_bottom_offset_ = 0.0f;
        }
    }

    // convert to cairo dimensions
    box_top_offset_ /= scale_;
    box_bottom_offset_ /= scale_;
    node_top_offset_ /= scale_;
    node_bottom_offset_ /= scale_;

    // we can now determine the size of the surface to draw the connection

    float conn_surface_w = ci::math<float>::abs(node_pos.x - box_position_.x) / scale_;
    float conn_surface_h = (node_top_offset_ + node_bottom_offset_);

    connection_surface_size_ = ci::Vec2f(conn_surface_w, conn_surface_h);

    connect_surface_ = std::shared_ptr<ci::cairo::SurfaceImage>(new 
        ci::cairo::SurfaceImage(conn_surface_w, conn_surface_h, true));

    connect_context_ = std::shared_ptr<ci::cairo::Context>(new
        ci::cairo::Context(*(connect_surface_.get())));

    connect_context_->setSourceRgb(node_.node_highlight_color().r, 
        node_.node_highlight_color().g, node_.node_highlight_color().b);

    context_setup_dash(connect_context_);

    if (is_left_of_node_) {
        connect_context_->line(ci::Vec2f(0.0f, box_top_offset_),
            ci::Vec2f(conn_surface_w, node_top_offset_));
        connect_context_->line(ci::Vec2f(0.0f, conn_surface_h - box_bottom_offset_),
            ci::Vec2f(conn_surface_w, node_top_offset_));
    } else { // is right of node
        connect_context_->line(ci::Vec2f(0.0f, node_top_offset_), 
            ci::Vec2f(conn_surface_w, box_top_offset_));
        connect_context_->line(ci::Vec2f(0.0f, node_top_offset_),
            ci::Vec2f(conn_surface_w, conn_surface_h - box_bottom_offset_));
    }

    connect_context_->stroke();

    connection_texture_ = ci::gl::Texture(connect_surface_->getSurface());

    if (is_left_of_node_) {
        if (is_below_node_ && !is_next_to_node_)
            ci::gl::draw(connection_texture_, ci::Vec2f(0.0f, -conn_surface_h + 
                surface_size_.y / 2.0f));
        else // is on top of node
            ci::gl::draw(connection_texture_, ci::Vec2f(0.0f, -surface_size_.y / 2.0f));
    } else { // is right of node
        if (is_below_node_ && !is_next_to_node_)
            ci::gl::draw(connection_texture_, ci::Vec2f(-conn_surface_w, 
            -conn_surface_h + surface_size_.y / 2.0f));
        else // is on top of node
            ci::gl::draw(connection_texture_, ci::Vec2f(-conn_surface_w, 
                -surface_size_.y / 2.0f));
    }
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

    context_setup_dash(context_);
    //context_setup_solid();

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

void CalloutBox::context_setup_dash(std::shared_ptr<ci::cairo::Context> ctx) {
    // scale_ division needed to transform from parent coordinates
    ctx->setLineCap(ci::cairo::LINE_CAP_ROUND);
    ctx->setLineWidth(line_width_);

    std::vector<double> dash;
    dash.push_back(0.0); // dash size
    dash.push_back(line_width_ * 2); // gap size

    ctx->setDash(dash);
}

void CalloutBox::context_setup_solid(std::shared_ptr<ci::cairo::Context> ctx) {
    ctx->setLineCap(ci::cairo::LINE_CAP_ROUND);
    ctx->setLineWidth(line_width_);
}

}

