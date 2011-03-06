
#include <assert.h>

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
#include "adj/adj_User.h"
#include "adj/adj_Renderer.h"

namespace adj {

const int CalloutBox::kMaxImageWidth = 120;
const int CalloutBox::kMaxImageHeight = 150;

int CalloutBox::max_width_ = 0;
int CalloutBox::max_height_ = 0;
float CalloutBox::max_scale_ = 0.0f;

CalloutBox::CalloutBox(GraphNode& parent) : node_(parent) {
    visible_ = true;
    text_color_ = ci::ColorA(1.0f, 0.0f, 1.0f, 1.0f);
    font_size_ = 48;
    
    is_left_of_node_ = false;
    side_margin_ = 40;
    top_margin_ = 30;
    text_spacing_ = 10;
    photo_spacing_ = 20;

    line_width_ = 8.0f;
    dash_gap_size_ = 2 * line_width_; // remember this accounts for circle size

    scale_ = 0.08f;

    alpha_ = 1.0f;
}

void CalloutBox::init() {
    box_coords_.resize(4);

    medium_font_ = ci::Font(ci::app::loadResource(RES_CROWTAP_FONT), 
        static_cast<float>(font_size_));

    hide();
}

CalloutBox::~CalloutBox() {
    // delete box particle
    GraphPhysics::instance().remove_box_particle(particle_);
}

void CalloutBox::update_contents() {
    if (!visible_)
        return;

    get_user_photos();
    calculate_surface_size();
    create_surface();
    create_context();
    update_box_position();

    set_contents();

    update_maxima();
}

void CalloutBox::update_maxima() {
    max_width_ = ci::math<int>::max(max_width_, surface_size_.x);
    max_height_ = ci::math<int>::max(max_height_, surface_size_.y);
    max_scale_ = ci::math<float>::max(max_scale_, scale_);
}

void CalloutBox::calculate_surface_size() {
    int width = calculate_surface_width(); 
    int num_photos = resized_user_photos_.size();

    int extra_vert_dim = 0;

    if (node_.is_current_song())
        extra_vert_dim = font_size_ + text_spacing_ * 3;


    int height = extra_vert_dim + top_margin_ * 2 + font_size_ * 3 + 
        text_spacing_ * 4 + num_photos * photo_spacing_; // extra 2 text spacing for "Voted by:" extra gap

    for (std::deque<ci::gl::Texture>::iterator it = resized_user_photos_.begin();
        it != resized_user_photos_.end(); ++it) {
        
        height += it->getHeight();
    }

    surface_size_ = ci::Vec2i(width, height);
}

void CalloutBox::create_surface() {
    surface_ = std::shared_ptr<ci::cairo::SurfaceImage>(new 
        ci::cairo::SurfaceImage(surface_size_.x, surface_size_.y, true));
}

void CalloutBox::create_context() {
    context_ = std::shared_ptr<ci::cairo::Context>(new ci::cairo::Context(*surface_));
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

float CalloutBox::get_width() {
    return surface_size_.x * scale_;
}

float CalloutBox::get_height() {
    return surface_size_.y * scale_;
}

float CalloutBox::get_pos_x() {
    return box_position_.x;
}

float CalloutBox::get_pos_y() {
    return box_position_.y;
}

void CalloutBox::draw() {
    if (visible_ == false)
        return;

    update_box_position();

    ci::gl::translate(box_position_);
    ci::gl::scale(ci::Vec3f::one() * scale_);

    //ci::gl::color(ci::Color::white());
    ci::gl::color(ci::ColorA(1.0f, 1.0f, 1.0f, alpha_));
    draw_connection();

    ci::Vec2f origin = ci::Vec2f(-surface_size_.x, -surface_size_.y) * 0.5f;
    ci::gl::draw(text_texture_, origin);

    std::deque<ci::Vec2f>::iterator pos_it = photo_coords_.begin();
    for (std::deque<ci::gl::Texture>::iterator text_it = resized_user_photos_.begin();
        text_it != resized_user_photos_.end(); ++text_it) {

        ci::gl::draw(*text_it, origin + *pos_it - text_it->getSize() * 0.5f);

        ++pos_it;
    }
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

    float surface_width = width / scale_ + line_width_ / scale_;
    float surface_height = height / scale_ + line_width_ / scale_;

    connect_surface_ = std::shared_ptr<ci::cairo::SurfaceImage>(new 
        ci::cairo::SurfaceImage(surface_width, surface_height, true));

    connect_context_ = std::shared_ptr<ci::cairo::Context>(new
        ci::cairo::Context(*connect_surface_));

    connect_context_->setSourceRgb(node_.node_highlight_color().r, 
        node_.node_highlight_color().g, node_.node_highlight_color().b);

    context_setup_dash(connect_context_);

    ci::Vec2f width_vec(line_width_, line_width_);

    surface_origin -= width_vec / 2.0f;

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

    if (particle_.get() == NULL)
        particle_ = GraphPhysics::instance().create_box_particle(node_.particle());

    update_contents();
}

void CalloutBox::hide() {
    visible_ = false;

    if (particle_.get() != NULL)
        GraphPhysics::instance().remove_box_particle(particle_);
    particle_.reset();
}

float CalloutBox::calculate_surface_width() {
    std::string max_name;
    bool user_name_max = true;

    for (std::deque<UserPtr>::iterator user_it = node_.song().users().begin();
        user_it != node_.song().users().end(); ++user_it) {
        if (max_name.length() > (*user_it)->name_.length())
            continue;

        max_name = (*user_it)->name_;
    }

    if (node_.song().name().length() > max_name.length()) {
        max_name = node_.song().name();
        user_name_max = false;
    }

    if (node_.song().artist().length() > max_name.length()) {
        max_name = node_.song().artist();
        user_name_max = false;
    }

    // CHANGE THIS IF THE TEXT IS GOING OUTSIDE CALLOUT BOX
    // ====================================================
    float letter_width = 2.2f / scale_;

    if (user_name_max)
        return kMaxImageWidth + photo_spacing_ * 2.f + 
            max_name.length() * letter_width +
            side_margin_ * 2.f;

    return max_name.length() * letter_width + side_margin_ * 2.f;
}

void CalloutBox::set_contents() {
    ci::Vec2f text_pos(side_margin_, top_margin_ + font_size_);
    ci::Vec2f font_height_offset(0.0f, font_size_ + text_spacing_);

    // draw background
    context_->setSourceRgb(Renderer::instance().background_color().r,
        Renderer::instance().background_color().g, 
        Renderer::instance().background_color().b);
    context_->rectangle(0.0, 0.0, surface_size_.x, surface_size_.y);
    context_->fill();

    // draw text
    context_->setFont(medium_font_);
    context_->setFontSize(static_cast<double>(font_size_));
    context_->setSourceRgb(1.0f, 1.0f, 1.0f);

    if (node_.is_current_song()) {
        context_->moveTo(text_pos);
        context_->showText("Now Playing:");
        text_pos += ci::Vec2f(0.0f, font_size_ + text_spacing_ * 3);
    }

    context_->moveTo(text_pos);
    context_->showText(node_.song().name());

    text_pos += font_height_offset;

    context_->moveTo(text_pos);
    context_->showText(node_.song().artist());

    text_pos += font_height_offset;
    text_pos += ci::Vec2f(0.0f, 2.f * text_spacing_);

    context_->moveTo(text_pos);
    context_->showText("Voted By:");

    text_pos += ci::Vec2f(0.0f, photo_spacing_);
    ci::Vec2f photo_pos = text_pos + ci::Vec2f(kMaxImageWidth / 2.0f, 0.0f);

    text_pos += ci::Vec2f(kMaxImageWidth + photo_spacing_ * 2, font_size_ / 2.0f);

    photo_coords_.clear();

    std::deque<ci::gl::Texture>::iterator texture_it = resized_user_photos_.begin();
    for (std::deque<UserPtr>::iterator user_it = node_.song().users().begin();
        user_it != node_.song().users().end(); ++user_it) {
        ci::Vec2f offset = ci::Vec2f(0.0f, texture_it->getHeight() / 2.0f);

        photo_pos += offset;
        text_pos += offset;

        photo_coords_.push_back(photo_pos);

        context_->moveTo(text_pos);
        context_->showText((*user_it)->name_);

        offset += ci::Vec2f(0.0f, photo_spacing_);

        text_pos += offset;
        photo_pos += offset;

        if (texture_it != resized_user_photos_.end())
            ++texture_it;
    }

    // draw boarder

    context_->setSourceRgb(node_.node_highlight_color().r, node_.node_highlight_color().g,
        node_.node_highlight_color().b);

    if (node_.is_current_song())
        context_setup_solid(context_);
    else
        context_setup_dash(context_);

    context_->newPath();
    context_->moveTo(line_width_, line_width_);
    context_->lineTo(surface_size_.x - line_width_, line_width_);
    context_->lineTo(surface_size_.x - line_width_, surface_size_.y - line_width_);
    context_->lineTo(line_width_, surface_size_.y - line_width_);
    context_->closePath();
    context_->stroke();
    
    text_texture_ = ci::gl::Texture(surface_->getSurface());
}

// create a new text texture
void CalloutBox::refresh_text() {
    ci::TextLayout layout;
    layout.setFont(ci::Font(ci::app::loadResource(RES_CROWTAP_FONT), 
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

void CalloutBox::get_user_photos() {
    resized_user_photos_.clear();

    for (std::deque<UserPtr>::iterator it = node_.song().users().begin();
        it != node_.song().users().end(); ++it) {
        // photo should be either be default photo or actual photo, not empty
        assert((*it)->photo_texture_resized_.getHeight() > 0 &&
            (*it)->photo_texture_resized_.getWidth() > 0);

        resized_user_photos_.push_back((*it)->photo_texture_resized_);
    }

    // every song is placed due to a vote, ever vote should have a user
    assert(!resized_user_photos_.empty());
}

}

