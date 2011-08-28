
#include <cinder/app/App.h>
#include <cinder/ImageIo.h>
#include <cinder/Surface.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/ip/Resize.h>

#include <AdjApp.h>
#include <adj/adj_CloudBackground.h>
#include <adj/adj_NowPlayingHeadline.h>

#include <Resources.h>

namespace adj {

CloudBackground* CloudBackground::instance_ = NULL;


CloudBackground::CloudBackground() {

}

void CloudBackground::init() {
    try {
        cloud_photo_ = ci::loadImage(ci::app::loadResource(RES_CLOUD_PHOTO));
    } catch (...) {
        ci::app::console() << "Unable to load cloud." << std::endl;
        AdjApp::instance().quit();
    }

    resize_photos();
}

void CloudBackground::resize_photos() {
    height_offset_ = NowPlayingHeadline::instance().size().y;

    int photo_width = cloud_photo_.getWidth();
    int photo_height = cloud_photo_.getHeight();

    int target_height = AdjApp::instance().getWindowHeight() - height_offset_;

    resized_photo_ = ci::ip::resizeCopy(cloud_photo_,
        ci::Area(0, 0, photo_width, photo_height), ci::Vec2i(
        static_cast<float>(target_height) / 
        static_cast<float>(photo_height) * static_cast<float>(photo_width),
        target_height));

    cloud_texture_ = ci::gl::Texture(resized_photo_);
}

void CloudBackground::draw() {
    ci::gl::color(ci::ColorA(1.0, 1.0, 1.0, 0.5));

    ci::gl::draw(cloud_texture_, ci::Vec2f(0, height_offset_));
}



CloudBackground& CloudBackground::instance() {
    if (instance_ == NULL) {
        instance_ = new CloudBackground();
        instance_->init();
    }

    return *instance_;
}

void CloudBackground::cleanup() {
    if (instance_ == NULL)
        return;

    delete instance_;
}


}