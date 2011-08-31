
#include <adj/adj_Adj.h>

#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/app/App.h>

#include <AdjApp.h>
#include <adj/adj_NowPlayingHeadline.h>
#include <adj/adj_PlayManager.h>
#include <adj/adj_GraphNode.h>
#include <adj/adj_Song.h>
#include <adj/adj_Renderer.h>

#include <Resources.h>

namespace adj {

NowPlayingHeadline* NowPlayingHeadline::instance_ = NULL;

NowPlayingHeadline::NowPlayingHeadline() {
    font_size_ = 45;
    top_offset_ = 8;
    side_offset_ = 15;
    
    dash_length_ = 10.0;
    
    border_line_width_ = 1.7;
    
    //top_grad_color_ = ci::Color(0.25, 0.25, 0.25);
    top_grad_color_ = ci::Color(Renderer::instance().background_color().r * 1.3f,
    Renderer::instance().background_color().g * 1.3f,
    Renderer::instance().background_color().b * 1.3f);
    
    corner_grad_color_ = ci::Color(0.15, 0.15, 0.15);
}

void NowPlayingHeadline::init() {
    try {
        text_font_ = ci::Font(ci::app::loadResource(RES_VM_FONT), 
            static_cast<float>(font_size_));
    } catch (...) {
        ci::app::console() << "Unable to set headline font." << std::endl;
        AdjApp::instance().quit();
        exit(-1); // really quit
    }
        
    surface_size_ = ci::Vec2i(AdjApp::instance().getWindowWidth(), 
        font_size_ * 2);
        
    //highlight_color_ = Renderer::instance().highlight_color();
    highlight_color_ = ci::Color::white();

    // create texture
    create_cairo_surface();
}

void NowPlayingHeadline::update() {
    return;

    if (now_playing_.get() == PlayManager::instance().now_playing().get())
        return;

    now_playing_ = PlayManager::instance().now_playing();

    song_name_ = now_playing_->song().name();
    artist_name_ = now_playing_->song().artist();
}

void NowPlayingHeadline::draw() {
    glColor3f(Renderer::instance().background_color()); 

    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(surface_size_.x, 0);
    glVertex2f(surface_size_.x, surface_size_.y);    
    glVertex2f(0, surface_size_.y);
    glEnd(); 

    ci::gl::color(ci::Color::white());
    ci::gl::draw(texture_, ci::Vec2f::zero());
}

void NowPlayingHeadline::create_cairo_surface() {
    surface_ = std::shared_ptr<ci::cairo::SurfaceImage>(new 
        ci::cairo::SurfaceImage(surface_size_.x,
        surface_size_.y, true));

    context_ = std::shared_ptr<ci::cairo::Context>(new ci::cairo::Context(*surface_));

    // draw background
    context_->setSourceRgb(Renderer::instance().background_color().r,
        Renderer::instance().background_color().g, 
        Renderer::instance().background_color().b);
    context_->rectangle(0.0, 0.0, surface_size_.x, surface_size_.y);
    context_->fill();

    context_->setSourceRgb(highlight_color_.r,
        highlight_color_.g, highlight_color_.b);

    context_->setLineCap(ci::cairo::LINE_CAP_SQUARE);
    context_->setLineWidth(border_line_width_);

    std::vector<double> dash;
    dash.push_back(dash_length_); // dash size
    dash.push_back(dash_length_ * 2); // gap size

    context_->setDash(dash);
    
    context_->newPath();
    context_->moveTo(border_line_width_, surface_size_.y - 
        border_line_width_);
    context_->lineTo(surface_size_.x - border_line_width_, surface_size_.y - 
        border_line_width_);
    //context_->lineTo(surface_size_.x - border_line_width_, border_line_width_);
    //context_->closePath();
    context_->stroke();
    
    /*
    context_->newPath();
    context_->moveTo(border_line_width_, border_line_width_);
    context_->lineTo(surface_size_.x - border_line_width_, border_line_width_);
    context_->lineTo(surface_size_.x - border_line_width_, surface_size_.y - 
        border_line_width_);
    context_->lineTo(border_line_width_, surface_size_.y - border_line_width_);
    context_->closePath();
    context_->stroke();
    */
    
    context_->setFont(text_font_);
    context_->setFontSize(static_cast<double>(font_size_));
    context_->setSourceRgb(highlight_color_.r,
        highlight_color_.g, highlight_color_.b);

    context_->moveTo(ci::Vec2f(side_offset_, top_offset_ + font_size_));

    context_->showText("Vote at vmworldparty.com");

    texture_ = ci::gl::Texture(surface_->getSurface());
}

void NowPlayingHeadline::remove_now_playing() {
    now_playing_.reset();
}

void NowPlayingHeadline::remove_now_playing(GraphNodePtr node) {
    if (node != now_playing_)
        return;

    now_playing_.reset();
}


NowPlayingHeadline& NowPlayingHeadline::instance() {
    if (instance_ == NULL) {
        instance_ = new NowPlayingHeadline();
        instance_->init();
    }

    return *instance_;
}


}