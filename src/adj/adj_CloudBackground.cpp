
#include <boost/date_time.hpp>

#include <cinder/app/App.h>
#include <cinder/ImageIo.h>
#include <cinder/Surface.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/ip/Resize.h>
#include <cinder/ip/Trim.h>
#include <cinder/Rand.h>
#include <cinder/CinderMath.h>

#include <AdjApp.h>
#include <adj/adj_CloudBackground.h>
#include <adj/adj_NowPlayingHeadline.h>

#include <Resources.h>

namespace adj {

CloudBackground* CloudBackground::instance_ = NULL;


CloudBackground::CloudBackground() {
    crop_clouds_ = true;
    cloud_1_timer_ = 0;
    cloud_2_timer_ = 0;
    cycle_length_ = 4000; // in ms
    max_alpha_ = 0.6;
}

void CloudBackground::init() {
    rand_.randomize();

    base_time_ = boost::posix_time::microsec_clock::universal_time();
    last_millis_ = get_millis();

    reset_timer(cloud_1_timer_);
    reset_timer(cloud_2_timer_);

    cloud_1_reset_ = cloud_1_timer_;
    cloud_2_reset_ = cloud_2_timer_;

    cloud_2_timer_ += cycle_length_ / 2;

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
    target_height_ = AdjApp::instance().getWindowHeight() - height_offset_;

    cloud_1_texture_ = generate_resized(cloud_photos_[rand_.randInt(4)]);
    cloud_2_texture_ = generate_resized(cloud_photos_[rand_.randInt(4)]);
}

ci::gl::Texture CloudBackground::generate_resized(ci::Surface& photo) {
    int photo_width = photo.getWidth();
    int photo_height = photo.getHeight();

    if (crop_clouds_) {
        ci::Area rand_area = get_random_area(photo);

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

    return ci::gl::Texture(resized_photo_);
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

void CloudBackground::update() {
    int m = get_millis();
    int diff = m - last_millis_;
    cloud_1_timer_ += diff;
    cloud_2_timer_ += diff;
    last_millis_ = m;

    cloud_1_alpha_ = get_alpha(cloud_1_timer_ - cloud_1_reset_);
    cloud_2_alpha_ = get_alpha(cloud_2_timer_ - cloud_2_reset_);

    if (cloud_1_timer_ - cloud_1_reset_ > cycle_length_) {
        cloud_1_reset_ = cloud_1_timer_;
        cloud_1_texture_ = generate_resized(cloud_photos_[rand_.randInt(4)]);
    }

    if (cloud_2_timer_ - cloud_2_reset_ > cycle_length_) {
        cloud_2_reset_ = cloud_2_timer_;
        cloud_2_texture_ = generate_resized(cloud_photos_[rand_.randInt(4)]);
    }
}

float CloudBackground::get_alpha(int t) {
    return ci::math<float>::abs(ci::math<float>::sin(static_cast<float>(
        t % cycle_length_) / cycle_length_ * M_PI)) * max_alpha_;
}

void CloudBackground::draw() {
    if (!show_clouds_)
        return;

    draw_cloud(cloud_1_texture_, cloud_1_alpha_);
    draw_cloud(cloud_2_texture_, cloud_2_alpha_);
}

void CloudBackground::draw_cloud(ci::gl::Texture& c, float a) {
    ci::gl::color(ci::ColorA(1.0, 1.0, 1.0, a));

    ci::gl::draw(c, ci::Vec2f(0, height_offset_));
}

int CloudBackground::get_millis() {
    boost::posix_time::time_duration diff = 
        boost::posix_time::microsec_clock::universal_time() - base_time_;

    return diff.total_milliseconds();
}

void CloudBackground::reset_timer(int& t) {
    t += cycle_length_;
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