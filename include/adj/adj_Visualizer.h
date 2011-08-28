
#pragma once

#include "adj/adj_Adj.h"

#include "cinder/app/App.h"

namespace graph {
    class ParticleSystem;
    class Particle;
}

namespace adj {

typedef std::shared_ptr<graph::Particle> ParticlePtr;
typedef std::shared_ptr<graph::ParticleSystem> ParticleSystemPtr;

class Visualizer {
public:
    Visualizer();
    ~Visualizer();

    void setup();
    void update();
    void draw();
    void shutdown();

    bool mouse_drag(ci::app::MouseEvent);
    bool key_down(ci::app::KeyEvent);
    bool resize(ci::app::ResizeEvent);

private:
    void add_node();

    ci::CallbackId mouse_drag_cb_id_;
    ci::CallbackId key_cb_id_;
    ci::CallbackId resize_id_;
};

}
