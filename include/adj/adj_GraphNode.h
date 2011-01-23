
#pragma once

#include <vector>

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "adj/adj_Adj.h"
#include "adj/adj_GraphNodeFactory.h"
#include "adj/adj_PlayManager.h"

namespace graph {
    class Particle;
}

namespace adj {

class Song;
typedef std::shared_ptr<Song> SongPtr;

typedef std::shared_ptr<graph::Particle> ParticlePtr;

class GraphNode;
typedef std::shared_ptr<GraphNode> GraphNodePtr;

class CalloutBox;
typedef std::shared_ptr<CalloutBox> CalloutBoxPtr;

typedef std::shared_ptr<struct Vote> VotePtr;

class GraphNode {
public:
    GraphNode();
    ~GraphNode();

    void init();

    void draw();

    // checks to see if the child has already been added 
    void add_child(GraphNodePtr);
    const std::vector<GraphNodePtr>& children() { return children_; }

    friend class GraphNodeFactory;

    Song& song() { return *(song_.get()); }

    bool is_current_song() { return is_current_song_; }
    bool is_next_song() { return is_next_song_; }
    bool is_transitioning() { return is_transitioning_; }
    bool is_transitioning_out() { return is_current_song_ && is_transitioning_; }
    bool is_transitioning_in() { return is_next_song_ && is_transitioning_; }

    GraphNodePtr parent() { return parent_; }

    void set_is_transitioning(bool trans);
    void set_is_current_song(bool current);
    void set_is_next_song(bool next);

    int distance_from_current() { return distance_from_current_; }

    const ci::ColorA& node_color() { return node_color_; }
    const ci::ColorA& node_highlight_color() { return node_highlight_color_; }
    
    float circle_radius() { return circle_radius_; }

    void update_appearance();

    ParticlePtr particle() { return particle_; }

    void register_vote(VotePtr);

    void show();
    void hide();

private:
    void draw_current_song();
    void draw_transitioning_out();
    void draw_transitioning_in();
    void draw_node(); // regular drawing
    void update_distance_from_current();

    SongPtr song_;
    ParticlePtr particle_;
    GraphNodePtr parent_;
    std::vector<GraphNodePtr> children_;

    CalloutBoxPtr callout_box_;

    bool is_current_song_;
    bool is_next_song_;
    bool is_transitioning_;
    int distance_from_current_;

    float scale_;

    float max_scale_;
    float min_scale_;

    ci::ColorA node_color_;
    ci::ColorA node_highlight_color_;

    bool is_valid_; // has a song that can be played
    bool visible_;

    float circle_radius_;
};

}
