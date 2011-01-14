
#pragma once

#include "adj/adj_Adj.h"

#include "cinder/app/App.h"

namespace graph {
    class ParticleSystem;
    class Particle;
}

namespace adj {

typedef std::tr1::shared_ptr<graph::Particle> ParticlePtr;
typedef std::tr1::shared_ptr<graph::ParticleSystem> ParticleSystemPtr;

class Visualizer {
public:
    Visualizer();
    ~Visualizer();

    void setup();
    void update();
    void draw();
    void shutdown();

    void setup_physics();

    bool mouse_drag(ci::app::MouseEvent);
    bool key_down(ci::app::KeyEvent);

private:
    void add_node();

    void update_centroid();
    void draw_network();

    ci::CallbackId mouse_drag_cb_id_;
    ci::CallbackId key_cb_id_;

    ParticleSystemPtr p_system_;

    float node_size_;

    float scale_;
    ci::Vec2f centroid_;
};

}
