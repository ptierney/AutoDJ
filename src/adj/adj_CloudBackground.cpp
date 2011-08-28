
#include <cinder/app/App.h>
#include <cinder/ImageIo.h>
#include <cinder/Surface.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/ip/Resize.h>
#include <cinder/ip/Trim.h>
#include <cinder/Rand.h>

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
        cloud_photos_.push_back(ci::loadImage(ci::app::loadResource(
            RES_CLOUD_PHOTO_1)));
        cloud_photos_.push_back(ci::loadImage(ci::app::loadResource(
            RES_CLOUD_PHOTO_2)));
        cloud_photos_.push_back(ci::loadImage(ci::app::loadResource(
            RES_CLOUD_PHOTO_3)));
        cloud_photos_.push_back(ci::loadImage(ci::app::loadResource(
            RES_CLOUD_PHOTO_4)));
    } catch (...) {
        ci::app::console() << "Unable to load cloud." << std::endl;
        AdjApp::instance().quit();
    }

    resize_photos();
}

void CloudBackground::resize_photos() {
    height_offset_ = NowPlayingHeadline::instance().size().y;

    ci::Rand rand;
    rand.randomize();

    generate_resized(cloud_photos_[rand.randInt(4)]);
}

void CloudBackground::generate_resized(ci::Surface& photo) {
    int photo_width = photo.getWidth();
    int photo_height = photo.getHeight();

    target_height_ = AdjApp::instance().getWindowHeight() - height_offset_;

    if (true) {
        ci::Area rand_area = get_random_area(photo);

        ci::app::console() << rand_area << std::endl;

        resized_photo_ = ci::Surface(AdjApp::instance().getWindowWidth(),
            target_height_, true, photo.getChannelOrder());

        resized_photo_.copyFrom(photo, rand_area, ci::Vec2i(-rand_area.x1,
            -rand_area.y1));
    } else {
        resized_photo_ = ci::ip::resizeCopy(photo,
            ci::Area(0, 0, photo_width, photo_height), ci::Vec2i(
            static_cast<float>(target_height_) / 
            static_cast<float>(photo_height) * static_cast<float>(photo_width),
            target_height_));
    }

    cloud_texture_ = ci::gl::Texture(resized_photo_);
}

ci::Area CloudBackground::get_random_area(ci::Surface& photo) {
    int photo_width = photo.getWidth();
    int photo_height = photo.getHeight();

    int x_range = photo.getWidth() - AdjApp::instance().getWindowWidth();
    int y_range = photo.getHeight() - target_height_;

    ci::Rand rand;
    rand.randomize();

    int rand_x = rand.randInt(x_range);
    int rand_y = rand.randInt(y_range);

    return ci::Area(rand_x, rand_y, AdjApp::instance().getWindowWidth() + rand_x,
        rand_y + target_height_);
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