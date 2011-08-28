
#pragma once

#include <vector>

#include <cinder/Surface.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>

namespace adj {

class CloudBackground {
public:
    static CloudBackground& instance();
    static void cleanup();

    void draw();

    void resize_photos();

private:
    CloudBackground();
    void init();
    void generate_resized(ci::Surface& photo);
    ci::Area get_random_area(ci::Surface& photo);

    int target_height_;

    static CloudBackground* instance_;

    std::vector<ci::Surface> cloud_photos_;

    ci::Surface resized_photo_;
    ci::gl::Texture cloud_texture_;

    int height_offset_;
};


}
