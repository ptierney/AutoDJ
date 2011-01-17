
#pragma once

#include <vector>

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "adj/adj_Adj.h"
#include "adj/adj_GraphNodeFactory.h"

namespace graph {
    class Particle;
}

namespace adj {

class Song;
typedef std::shared_ptr<Song> SongPtr;

typedef std::shared_ptr<graph::Particle> ParticlePtr;

class GraphNode;
typedef std::shared_ptr<GraphNode> GraphNodePtr;

class GraphNode {
public:
    GraphNode();

    void init();

    void draw();

    // checks to see if the child has already been added 
    void add_child(GraphNodePtr);
    const std::vector<GraphNodePtr>& children() { return children_; }

    friend class GraphNodeFactory;

    Song& song() { return *(song_.get()); }

    void refresh_text();

private:
    SongPtr song_;
    ParticlePtr particle_;
    GraphNodePtr parent_;
    std::vector<GraphNodePtr> children_;

    float scale_;

    std::string text_font_;
    std::string text_font_i_;
    int font_size_;
    ci::ColorA node_color_;
    ci::ColorA text_color_;
    ci::gl::Texture text_texture_;

    bool is_valid_; // has a song that can be played

    float size_;
};

}