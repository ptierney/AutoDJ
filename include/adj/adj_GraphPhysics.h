
#pragma once

#include "adj/adj_Adj.h"

#include "cinder/Vector.h"

namespace graph {
    class ParticleSystem;
    class Particle;
}

namespace adj {

typedef std::tr1::shared_ptr<graph::Particle> ParticlePtr;
typedef std::tr1::shared_ptr<graph::ParticleSystem> ParticleSystemPtr;

class GraphPhysics {
public:
    static GraphPhysics& instance();
    static void cleanup();

    ParticlePtr create_node(); // Attatch the node to a random node
    ParticlePtr create_node(ParticlePtr parent); // create new attatched to parent

    void update();

    ParticleSystemPtr particle_system() { return p_system_; }

private:
    GraphPhysics();
    void init();

    void setup_new_node(ParticlePtr p, ParticlePtr parent);
    void add_spacers_to_node(ParticlePtr p, ParticlePtr r);
    void make_edge_between(ParticlePtr a, ParticlePtr b);

    // shared_ptrs to all sorts of classes
    ParticleSystemPtr p_system_;

    static GraphPhysics* instance_;

    float spacer_strength_;
    float edge_strength_;
    float edge_length_;
};

}
