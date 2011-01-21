
#pragma once

// CalloutBoxes display text about the song, it's place in the queue,
// and the names and faces of the people who voted on the song

#include "adj/adj_Adj.h"

#include <string>

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Color.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Text.h"
#include "cinder/Font.h"

namespace adj {

class GraphNode;

typedef std::shared_ptr<class graph::Particle> ParticlePtr;

class CalloutBox {
public:
    CalloutBox(GraphNode& parent);
    void init();

    void update_contents();

    void draw();

    bool visible() { return visible_; }
    void show();
    void hide();

private:
    void set_contents_current();
    void set_contents_node();
    void set_contents_just_added();
    void context_setup_dash(std::shared_ptr<ci::cairo::Context>);
    void context_setup_solid(std::shared_ptr<ci::cairo::Context>);
    void update_box_position();

    void calculate_surface_size();
    void create_surface();
    void create_context();

    void draw_connection();
    void create_connect_surface();
    void create_connect_context();
    void render_connection();

    void refresh_text(); // temporary

    GraphNode& node_;
    bool visible_;

    int font_size_;
    ci::ColorA text_color_;
    ci::gl::Texture text_texture_;
    ci::gl::Texture connection_texture_;

    float line_width_;
    float dash_gap_size_;

    ci::Vec2f surface_size_;
    ci::Vec2f connection_surface_size_;
    int side_margin_;
    int top_margin_;
    int text_spacing_;

    bool is_left_of_node_;
    bool is_below_node_;
    bool is_next_to_node_;

    ci::Vec2f box_position_;
    float box_top_y_;
    float box_bottom_y_;

    ci::Font medium_font_;
    // I can't seem to figure out how to use italic fonts with cairo atm
    //ci::Font italic_font_;
    //ci::Font bold_font_;

    float scale_;

    std::shared_ptr<ci::cairo::SurfaceImage> surface_;
    std::shared_ptr<ci::cairo::Context> context_;

    std::shared_ptr<ci::cairo::SurfaceImage> connect_surface_;
    std::shared_ptr<ci::cairo::Context> connect_context_;

    ParticlePtr particle_;
};

}
