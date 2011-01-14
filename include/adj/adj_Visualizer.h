
#pragma once

#include "cinder/Cinder.h"

#if defined (CINDER_MAC)
    #include <tr1/memory>
#endif

#include "cinder/app/App.h"


namespace graph {
    class ParticleSystem;
    class Particle;
}

namespace adj {

typedef std::tr1::shared_ptr<graph::Particle> ParticlePtr;

class Visualizer {
public:
    Visualizer();
    ~Visualizer();

    void setup();
    void update();
    void draw();

    void setup_physics();

    bool mouse_drag(ci::app::MouseEvent);
    bool key_down(ci::app::KeyEvent);

private:
    void add_node();
    void add_spacers_to_node(ParticlePtr p, ParticlePtr r);
    void make_edge_between(ParticlePtr a, ParticlePtr b);
    void update_centroid();
    void draw_network();

    // shared_ptrs to all sorts of classes
    std::tr1::shared_ptr<graph::ParticleSystem> p_system_;

    ci::CallbackId mouse_drag_cb_id_;
    ci::CallbackId key_cb_id_;

    float spacer_strength_;
    float edge_strength_;
    float edge_length_;
    float node_size_;

    float scale_;
    ci::Vec2i centroid_;
};

}
