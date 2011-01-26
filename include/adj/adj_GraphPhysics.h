
#pragma once

#include "adj/adj_Adj.h"

#include <map>

#include "cinder/Vector.h"

namespace graph {
    class ParticleSystem;
    class Particle;
}

namespace adj {

typedef std::shared_ptr<graph::Particle> ParticlePtr;
typedef std::shared_ptr<graph::ParticleSystem> ParticleSystemPtr;
typedef std::shared_ptr<class GraphNode> GraphNodePtr;

class GraphPhysics {
public:
    static GraphPhysics& instance();
    static void cleanup();

    // NOTE: the notion of a parent-child relationship isn't strict in this
    // physics model: specifying a parent simply means that the two node are
    // connected by a spring; there's no sense of directionality (that's set 
    // up in the GraphNode object.
    ParticlePtr create_unconnected_particle();
    ParticlePtr create_particle_connected_randomly(); 
    //ParticlePtr create_particle(); // 
    ParticlePtr create_particle(ParticlePtr parent); // create new attatched to parent
    ParticlePtr create_particle(ParticlePtr parent, float length); // create new attatched to parent
    ParticlePtr create_particle(ParticlePtr parent, float length, float strength); // create new attatched to parent

    // make a particle, add separators, but don't link it
    ParticlePtr create_unlinked_particle(ParticlePtr parent);
    void link_nodes(GraphNodePtr a, GraphNodePtr b);
    void link_nodes(GraphNodePtr a, GraphNodePtr b, float length, float strength);

    // Particles are attatched by a spring to their parent,
    // and have separators to all other boxes.
    ParticlePtr create_box_particle(ParticlePtr parent);

    void update();

    ParticleSystemPtr particle_system() { return p_system_; }
    ParticleSystemPtr box_particle_system() { return box_p_system_; }

    float edge_length() { return edge_length_; }

private:
    GraphPhysics();
    void init();

    void setup_new_node(ParticlePtr p, ParticlePtr parent, float length, float strength);
    void add_spacers_to_node(ParticleSystemPtr, ParticlePtr p, ParticlePtr r);
    void make_edge_between(ParticleSystemPtr, ParticlePtr a, ParticlePtr b);
    void make_edge_between(ParticleSystemPtr, ParticlePtr a, ParticlePtr b, float length,
        float strength);
    void make_separation_between(ParticleSystemPtr, ParticlePtr a, ParticlePtr b);
    // this updates the box particle system, but also makes sure the 
    // box's node particle is anchored to the node in the graph 
    void update_box_particles();

    std::vector<ParticlePtr> boxes_;
    // this is the mapping from a box's node particle
    // to the actual particle on the song graph
    std::map<ParticlePtr, ParticlePtr> box_p_to_anchor_p_map_;
    std::map<ParticlePtr, ParticlePtr> box_anchor_to_graph_map_;


    // shared_ptrs to all sorts of classes
    ParticleSystemPtr p_system_;
    ParticleSystemPtr box_p_system_;

    static GraphPhysics* instance_;

    float spacer_strength_;
    float edge_strength_;
    float edge_length_;
};

}
