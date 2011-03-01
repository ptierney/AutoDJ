
#pragma once

#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <cinder/gl/gl.h>
#include <cinder/gl/Texture.h>

#include <adj/adj_Adj.h>
#include <adj/adj_GraphNodeFactory.h>
#include <adj/adj_PlayManager.h>
#include <adj/adj_GraphicItem.h>

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

class GraphNode : public GraphicItem {
public:
    GraphNode();
    ~GraphNode();

    void init();

    void draw();

    float get_width();
    float get_height();
    float get_pos_x();
    float get_pos_y();

    // checks to see if the child has already been added 
    void add_child(GraphNodePtr);
    const std::vector<GraphNodePtr>& children() { return children_; }

    friend class GraphNodeFactory;

    Song& song() { return *song_; }

    bool is_current_song() { return is_current_song_; }
    bool is_next_song() { return is_next_song_; }
    bool is_transitioning() { return is_transitioning_; }
    bool is_transitioning_out() { return is_current_song_ && is_transitioning_; }
    bool is_transitioning_in() { return is_next_song_ && is_transitioning_; }

    bool highlight_connection();

    GraphNodePtr parent() { return parent_; }

    void set_is_transitioning(bool trans);
    void set_is_current_song(bool current);
    void set_is_next_song(bool next);
    void register_song_playing();
    void register_just_added();
    void register_path_activate();

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
    void draw_node(); // regular drawing
    void update_distance_from_current();
    int get_milliseconds_elapsed(boost::posix_time::ptime&);
    void check_transition_states();
    void transition_fading_in();
    void transition_fading_out();
    void check_display_time();
    void check_path_delay();
    void start_callout_fadein();
    void start_callout_fadeout();
    void trigger_next_node();

    SongPtr song_;
    ParticlePtr particle_;
    GraphNodePtr parent_;
    std::vector<GraphNodePtr> children_;

    CalloutBoxPtr callout_box_;

    bool is_current_song_;
    bool is_next_song_;
    bool is_transitioning_;
    int distance_from_current_;

    bool is_fading_in_;
    bool is_fading_out_;
    bool is_path_delaying_;
    int fade_time_; // in milliseconds
    int display_time_; // in milliseconds
    int path_trigger_delay_time_; // in milliseconds
    boost::posix_time::ptime fade_timer_;
    boost::posix_time::ptime display_timer_;
    boost::posix_time::ptime path_trigger_timer_;

    float scale_;

    float max_scale_;
    float min_scale_;

    ci::ColorA node_color_;
    ci::ColorA node_highlight_color_;

    bool is_valid_; // has a song that can be played
    bool visible_;

    float circle_radius_;

    ci::Surface highlight_circle_;
    ci::gl::Texture highlight_circle_texture_;
    float circle_texture_scale_;
};

}
