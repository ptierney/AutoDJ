
#pragma once

#include <vector>

#include <boost/date_time.hpp>

#include <cinder/Surface.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/Rand.h>

namespace adj {

class CloudBackground {
public:
    static CloudBackground& instance();
    static void cleanup();

    void update();
    void draw();

    void resize_photos();

    bool& show_clouds() { return show_clouds_; }

private:
    CloudBackground();
    void init();
    ci::gl::Texture generate_resized(ci::Surface& photo);
    ci::Area get_random_area(ci::Surface& photo);
    void reset_timer(int& t);
    void draw_cloud(ci::gl::Texture& text, float a);
    float get_alpha(int t);

    bool show_clouds_;

    int target_height_;

    static CloudBackground* instance_;

    std::vector<ci::Surface> cloud_photos_;

    ci::Surface resized_photo_;
    ci::gl::Texture cloud_1_texture_;
    ci::gl::Texture cloud_2_texture_;

    bool crop_clouds_;

    int cycle_length_; // in ms

    int cloud_1_timer_;
    int cloud_2_timer_;

    int cloud_1_reset_;
    int cloud_2_reset_;

    float cloud_1_alpha_;
    float cloud_2_alpha_;

    boost::posix_time::ptime base_time_;

    int get_millis();
    int last_millis_;

    float max_alpha_;

    ci::Rand rand_;

    int height_offset_;
};


}
