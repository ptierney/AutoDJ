
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

const int CalloutBox::kMaxImageWidth = 120;
const int CalloutBox::kMaxImageHeight = 150;

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
    box_coords_.resize(4);

    medium_font_ = ci::Font(ci::app::loadResource(RES_MEDIUM_FONT), font_size_);

    //particle_ = GraphPhysics::instance().create_box_particle(node_.particle());
    particle_ = GraphPhysics::instance().create_particle(node_.particle(),
        GraphPhysics::instance().edge_length() * 2.0f);

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
    // make the surface centered on the particle
    box_position_ = particle_->position();

    box_upper_left_ = box_position_ + ci::Vec2f(-surface_size_.x,
        -surface_size_.y) * 0.5f * scale_;
    box_upper_right_ = box_position_ + ci::Vec2f(surface_size_.x,
        -surface_size_.y) * 0.5f * scale_;
    box_lower_left_ = box_position_ + ci::Vec2f(-surface_size_.x,
        surface_size_.y) * 0.5f * scale_;
    box_lower_right_ = box_position_ +ci::Vec2f(surface_size_.x,
        surface_size_.y) * 0.5f * scale_;

    box_coords_[0] = box_upper_left_;
    box_coords_[1] = box_upper_right_;
    box_coords_[2] = box_lower_right_;
    box_coords_[3] = box_lower_left_;
}

void CalloutBox::draw() {
    if (visible_ == false)
        return;

    update_box_position();

    ci::gl::translate(box_position_);
    ci::gl::scale(ci::Vec3f::one() * scale_);

    ci::gl::color(ci::Color::white());

    draw_connection();

    ci::gl::draw(text_texture_, ci::Vec2f(-surface_size_.x, 
        -surface_size_.y) * 0.5f);
}

void CalloutBox::draw_connection() {
    render_connection();
}

void CalloutBox::render_connection() {
    connection_ends_.clear();

    ci::Vec2f node_pos = node_.particle()->position();

    for (int i = 0; i < 4; ++i) {
        if (!segments_intersect(node_pos, box_coords_[i],  box_coords_[(i+1) % 4],
            box_coords_[(i+2) % 4]) && !segments_intersect(node_pos, box_coords_[i],  
            box_coords_[(i+2) % 4], box_coords_[(i+3) % 4])) {

            add_connection_end(i);
        }
    }

    ci::Vec2f a = box_coords_[connection_ends_[0]];
    ci::Vec2f b = box_coords_[connection_ends_[1]];

    float width = ci::math<float>::max(ci::math<float>::max(
        ci::math<float>::abs(node_pos.x - a.x),
        ci::math<float>::abs(node_pos.x - b.x)),
        ci::math<float>::abs(a.x - b.x));

    float height = ci::math<float>::max(ci::math<float>::max(
        ci::math<float>::abs(node_pos.y - a.y),
        ci::math<float>::abs(node_pos.y - b.y)),
        ci::math<float>::abs(a.y - b.y));

    float leftmost_x = ci::math<float>::min(ci::math<float>::min(
        node_pos.x, a.x), b.x);

    float topmost_y = ci::math<float>::min(ci::math<float>::min(
        node_pos.y, a.y), b.y);

    ci::Vec2f surface_origin(leftmost_x, topmost_y);

    float surface_width = width / scale_;
    float surface_height = height / scale_;

    connect_surface_ = std::shared_ptr<ci::cairo::SurfaceImage>(new 
        ci::cairo::SurfaceImage(surface_width, surface_height, true));

    connect_context_ = std::shared_ptr<ci::cairo::Context>(new
        ci::cairo::Context(*(connect_surface_.get())));

    connect_context_->setSourceRgb(node_.node_highlight_color().r, 
        node_.node_highlight_color().g, node_.node_highlight_color().b);

    context_setup_dash(connect_context_);

    connect_context_->line((node_pos - surface_origin) / scale_,
        (a - surface_origin) / scale_);
    connect_context_->line((node_pos - surface_origin) / scale_,
        (b - surface_origin) / scale_);

    connect_context_->stroke();

    connection_texture_ = ci::gl::Texture(connect_surface_->getSurface());

    ci::gl::draw(connection_texture_, 
        (surface_origin - particle_->position()) / scale_);
}

void CalloutBox::add_connection_end(int a) {
    if (connection_ends_.size() < 2) {
        connection_ends_.push_back(a);
        return;
    }

    float dist_a = box_coords_[a].distanceSquared(node_.particle()->position());
    float dist_0 = box_coords_[connection_ends_[0]].distanceSquared(node_.particle()->position());
    float dist_1 = box_coords_[connection_ends_[1]].distanceSquared(node_.particle()->position());

    if (dist_0 > dist_a && dist_1 > dist_a)
        return;
        
    if (dist_0 < dist_1)
        connection_ends_[0] = a;
    else
        connection_ends_[1] = a;
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

    ci::Vec2f text_pos(side_margin_, top_margin_ + font_size_);
    ci::Vec2f font_height_offset(0.0f, font_size_ + text_spacing_);
    float image_width = 

    context_->setFont(medium_font_);
    context_->setFontSize(static_cast<double>(font_size_));
    context_->setSourceRgb(1.0f, 1.0f, 1.0f);
    context_->moveTo(text_pos);
    context_->showText(boost::lexical_cast<std::string>(node_.distance_from_current()));

    text_pos += font_height_offset;

    context_->moveTo(text_pos);
    context_->setFont(medium_font_);    context_->showText(node_.song().name());

    text_pos += font_height_offset;

    context_->moveTo(text_pos);
    context_->showText(node_.song().artist());

    text_pos


    // draw boarder

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

void CalloutBox::resize_user_photos() {
    resized_user_photos_

    node_.song().users()
}

}

