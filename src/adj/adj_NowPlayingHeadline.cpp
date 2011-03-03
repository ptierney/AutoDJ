
#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/app/App.h>

#include <AdjApp.h>
#include <adj/adj_NowPlayingHeadline.h>
#include <adj/adj_PlayManager.h>
#include <adj/adj_GraphNode.h>
#include <adj/adj_Song.h>

#include <Resources.h>

namespace adj {

NowPlayingHeadline* NowPlayingHeadline::instance_ = NULL;

NowPlayingHeadline::NowPlayingHeadline() {
    font_size_ = 45;
    top_offset_ = 8;
    side_offset_ = 8;
}

void NowPlayingHeadline::init() {
    text_font_ = ci::Font(ci::app::loadResource(RES_MEDIUM_FONT), 
        static_cast<float>(font_size_));
}

void NowPlayingHeadline::update() {
    if (now_playing_.get() == PlayManager::instance().now_playing().get())
        return;

    now_playing_ = PlayManager::instance().now_playing();

    song_name_ = now_playing_->song().name();
    artist_name_ = now_playing_->song().artist();

    // create texture
    create_cairo_surface();
}

void NowPlayingHeadline::draw() {
    if (now_playing_.get() == NULL)
        return;

    ci::gl::color(ci::Color::white());
    ci::gl::draw(texture_, ci::Vec2f(top_offset_, side_offset_));
}

void NowPlayingHeadline::create_cairo_surface() {
    surface_ = std::shared_ptr<ci::cairo::SurfaceImage>(new 
        ci::cairo::SurfaceImage(AdjApp::instance().getWindowWidth(), 
        font_size_ * 2, true));

    context_ = std::shared_ptr<ci::cairo::Context>(new ci::cairo::Context(*surface_));

    context_->setFont(text_font_);
    context_->setFontSize(static_cast<double>(font_size_));
    context_->setSourceRgb(1.0f, 1.0f, 1.0f);

    context_->moveTo(ci::Vec2f(0, font_size_));

    context_->showText("Now Playing: " + song_name_ + " - " + artist_name_);

    texture_ = ci::gl::Texture(surface_->getSurface());
}

NowPlayingHeadline& NowPlayingHeadline::instance() {
    if (instance_ == NULL) {
        instance_ = new NowPlayingHeadline();
        instance_->init();
    }

    return *instance_;
}


}