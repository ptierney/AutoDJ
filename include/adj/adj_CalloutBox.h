
#pragma once

// CalloutBoxes display text about the song, it's place in the queue,
// and the names and faces of the people who voted on the song

#include <string>

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Color.h"

namespace adj {

class GraphNode;

class CalloutBox {
public:
    CalloutBox(GraphNode& parent);
    void init();

    void update_contents();

    void draw();

    bool visible() { return visible_; }
    void show();
    void hide();

private:
    void set_contents_current();
    void set_contents_node();
    void set_contents_just_added();

    void refresh_text(); // temporary

    GraphNode& node_;
    bool visible_;

    std::string text_font_;
    std::string text_font_i_;
    int font_size_;
    ci::ColorA text_color_;
    ci::gl::Texture text_texture_;
};

}
