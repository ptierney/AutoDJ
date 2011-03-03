
#pragma once

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

private:
    void create_cairo_surface();

    GraphNodePtr now_playing_;
    std::string song_name_;
    std::string artist_name_;

    ci::Font text_font_;
    int font_size_;
    int top_offset_;
    int side_offset_;

    ci::gl::Texture texture_;
    std::shared_ptr<ci::cairo::SurfaceImage> surface_;
    std::shared_ptr<ci::cairo::Context> context_;

    static NowPlayingHeadline* instance_;
};

}