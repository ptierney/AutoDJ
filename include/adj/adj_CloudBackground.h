
#pragma once

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

    static CloudBackground* instance_;

    ci::Surface cloud_photo_;
    ci::Surface resized_photo_;
    ci::gl::Texture cloud_texture_;

    int height_offset_;
};


}
