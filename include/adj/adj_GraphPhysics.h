
#pragma once

#include "adj/adj_Adj.h"

#include "cinder/Vector.h"

namespace graph {
    class ParticleSystem;
    class Particle;
}

namespace adj {

typedef std::shared_ptr<graph::Particle> ParticlePtr;
typedef std::shared_ptr<graph::ParticleSystem> ParticleSystemPtr;

class GraphPhysics {
public:
    static GraphPhysics& instance();
    static void cleanup();

    // NOTE: the notion of a parent-child relationship isn't strict in this
    // physics model: specifying a parent simply means that the two node are
    // connected by a spring; there's no sense of directionality (that's set 
    // up in the GraphNode object.
    ParticlePtr create_unconnected_particle();
    ParticlePtr create_particle(); // Attatch the node to a random node
    ParticlePtr create_particle(ParticlePtr parent); // create new attatched to parent
    ParticlePtr create_particle(ParticlePtr parent, float length); // create new attatched to parent
    ParticlePtr create_particle(ParticlePtr parent, float length, float strength); // create new attatched to parent

    // Particles are attatched by a spring to their parent,
    // and have separators to all other boxes.
    ParticlePtr create_box_particle(ParticlePtr parent);

    void update();

    ParticleSystemPtr particle_system() { return p_system_; }

    float edge_length() { return edge_length_; }

private:
    GraphPhysics();
    void init();

    void setup_new_node(ParticlePtr p, ParticlePtr parent, float length, float strength);
    void add_spacers_to_node(ParticlePtr p, ParticlePtr r);
    void make_edge_between(ParticlePtr a, ParticlePtr b);
    void make_edge_between(ParticlePtr a, ParticlePtr b, float length,
        float strength);
    void make_separation_between(ParticlePtr a, ParticlePtr b);

    std::vector<ParticlePtr> boxes_;

    // shared_ptrs to all sorts of classes
    ParticleSystemPtr p_system_;

    static GraphPhysics* instance_;

    float spacer_strength_;
    float edge_strength_;
    float edge_length_;
};

}
