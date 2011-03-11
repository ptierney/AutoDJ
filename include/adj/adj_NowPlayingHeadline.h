
#pragma once

/**************
 * OK, so tihs isn't really a strict
 * 'Now Playing' bar, just a URL.
 ***************************/

#include <adj/adj_Adj.h>

#include <string>

#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>
#include <cinder/cairo/Cairo.h>

namespace adj {

typedef std::shared_ptr<class GraphNode> GraphNodePtr;

class NowPlayingHeadline {
public:
    NowPlayingHeadline();
    void init();

    void update();
    void draw();

    static NowPlayingHeadline& instance();

    void remove_now_playing();
    void remove_now_playing(GraphNodePtr);
    
    ci::Vec2i size() { return surface_size_; }

private:
    void create_cairo_surface();

    GraphNodePtr now_playing_;
    std::string song_name_;
    std::string artist_name_;

    ci::Font text_font_;
    int font_size_;
    int top_offset_;
    int side_offset_;
    
    double dash_length_;
    
    ci::Vec2i surface_size_;
    double border_line_width_;
    ci::Color highlight_color_;
    ci::Color top_grad_color_;
    ci::Color corner_grad_color_;

    ci::gl::Texture texture_;
    std::shared_ptr<ci::cairo::SurfaceImage> surface_;
    std::shared_ptr<ci::cairo::Context> context_;

    static NowPlayingHeadline* instance_;
};

}