
#pragma once

#include "cinder/Color.h"

namespace graph {
    class Particle;
    class ParticleSystem;
}

namespace adj {

typedef std::shared_ptr<graph::Particle> ParticlePtr;
typedef std::shared_ptr<graph::ParticleSystem> ParticleSystemPtr;

class Renderer {
public:
    static Renderer& instance();
    static void cleanup();

    void setup();
    void draw();

private:
    Renderer();
    void init();

    void draw_nodes();
    void draw_connections();

    static Renderer* instance_;

    ci::Color background_color_;
    ci::ColorA network_color_;

    float node_size_;

    ParticleSystemPtr p_system_;

};

}