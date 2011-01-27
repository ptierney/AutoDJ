
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

    // in pixels
    static const int kMaxImageWidth;
    static const int kMaxImageHeight;

    static float max_width() { return max_width_ * max_scale_; }
    static float max_height() { return max_height_ * max_scale_; }
    static float max_scale() { return max_scale_; }

    void set_alpha(float a) { alpha_ = a; }

private:
    void set_contents();
    void context_setup_dash(std::shared_ptr<ci::cairo::Context>);
    void context_setup_solid(std::shared_ptr<ci::cairo::Context>);
    void update_box_position();
    void get_user_photos();
    void update_maxima();

    void calculate_surface_size();
    void create_surface();
    void create_context();

    void draw_connection();
    void render_connection();

    bool ccw(const ci::Vec2f& a, const ci::Vec2f& b, const ci::Vec2f& c) {
        return (c.y-a.y)*(b.x-a.x) > (b.y-a.y)*(c.x-a.x);
    }
    // segment ab intersects cd
    bool segments_intersect(const ci::Vec2f& a, const ci::Vec2f& b, 
        const ci::Vec2f& c, const ci::Vec2f& d) {
            return (ccw(a, c, d) != ccw(b, c, d)) && 
                (ccw(a, b, c) != ccw(a, b, d));
    }

    void add_connection_end(int);
    std::vector<int> connection_ends_;

    void refresh_text(); // temporary

    GraphNode& node_;
    bool visible_;

    int font_size_;
    ci::ColorA text_color_;
    ci::gl::Texture text_texture_;
    ci::gl::Texture connection_texture_;
    float alpha_;

    float line_width_;
    float dash_gap_size_;

    ci::Vec2i surface_size_;
    ci::Vec2i connection_surface_size_;
    int side_margin_;
    int top_margin_;
    int text_spacing_;
    int photo_spacing_;

    bool is_left_of_node_;
    bool is_below_node_;
    bool is_next_to_node_;

    ci::Vec2f box_position_;
    float box_top_y_;
    float box_bottom_y_;
    ci::Vec2f box_upper_left_;
    ci::Vec2f box_upper_right_;
    ci::Vec2f box_lower_left_;
    ci::Vec2f box_lower_right_;
    std::vector<ci::Vec2f> box_coords_;

    ci::Font medium_font_;
    // I can't seem to figure out how to use italic fonts with cairo atm
    //ci::Font italic_font_;
    //ci::Font bold_font_;

    float scale_;
    static float max_scale_;

    std::shared_ptr<ci::cairo::SurfaceImage> surface_;
    std::shared_ptr<ci::cairo::Context> context_;

    std::shared_ptr<ci::cairo::SurfaceImage> connect_surface_;
    std::shared_ptr<ci::cairo::Context> connect_context_;

    std::deque<ci::gl::Texture> resized_user_photos_;
    std::deque<ci::Vec2f> photo_coords_;

    ParticlePtr particle_;

    static int max_width_;
    static int max_height_;
};

}
