
#include "cinder/Vector.h"
#include "cinder/Rand.h"

#include "graph/graph_ParticleSystem.h"
#include "graph/graph_Particle.h"

#include "adj/adj_GraphPhysics.h"
#include "adj/adj_GraphNode.h"

namespace adj {

GraphPhysics* GraphPhysics::instance_ = NULL;

GraphPhysics::GraphPhysics() {
    spacer_strength_ = 1000.0f;
    edge_length_ = 20.0f;
    edge_strength_ = 0.2f;
}

void GraphPhysics::init() {
    p_system_ = ParticleSystemPtr(new graph::ParticleSystem(ci::Vec2f::zero(), 0.1f));
    p_system_->init();
    p_system_->clear();

    box_p_system_ = ParticleSystemPtr(new graph::ParticleSystem(ci::Vec2f::zero(), 0.1f));
    box_p_system_->init();
    box_p_system_->clear();
}

void GraphPhysics::update() {
    p_system_->tick();

    update_box_particles();
}

ParticlePtr GraphPhysics::create_unconnected_particle() {
    return p_system_->make_particle();
}

ParticlePtr GraphPhysics::create_particle_connected_randomly() {
    ci::Rand rand;
    rand.randomize();

    ParticlePtr p = p_system_->make_particle();

    ParticlePtr q;
    do {
        q = p_system_->particles_[rand.randInt(p_system_->particles_.size() - 1)];
    } while (p.get() == q.get());

    setup_new_node(p, q, edge_length_, edge_strength_);

    return p;
}

//ParticlePtr GraphPhysics::create_particle() {
//}

ParticlePtr GraphPhysics::create_particle(ParticlePtr q) {
    return create_particle(q, edge_length_);
}

ParticlePtr GraphPhysics::create_particle(ParticlePtr q, float length) {
    return create_particle(q, length, edge_strength_);
}

ParticlePtr GraphPhysics::create_particle(ParticlePtr q, float length,
    float strength) {
    ParticlePtr p = p_system_->make_particle();

    setup_new_node(p, q, length, strength);

    return p;
}

ParticlePtr GraphPhysics::create_box_particle(ParticlePtr parent) {
    ParticlePtr box_p = box_p_system_->make_particle();
    ParticlePtr node_p = box_p_system_->make_particle();

    node_p->position() = ci::Vec2f(parent->position()); // TODO: check needed

    box_p_to_anchor_p_map_[box_p] = node_p;
    box_anchor_to_graph_map_[node_p] = parent;

    // separate the box from all other boxes
    for (std::vector<ParticlePtr>::iterator it = boxes_.begin(); it != boxes_.end(); ++it) {
        make_separation_between(box_p_system_, box_p, *it);
    }

    boxes_.push_back(box_p);

    make_separation_between(box_p_system_, box_p, node_p);
    make_edge_between(box_p_system_, box_p, node_p, edge_length_ * 2.0f, 
        edge_strength_);

    ci::Rand rand;
    rand.randomize();

    box_p->position() = parent->position() + ci::Vec2f(rand.randFloat(-1.0f, 1.0f),
        rand.randFloat(-1.0f, 1.0f));

    return box_p;
}

void GraphPhysics::update_box_particles() {
    box_p_system_->tick();

    for (std::map<ParticlePtr, ParticlePtr>::iterator it = 
        box_anchor_to_graph_map_.begin(); it != box_anchor_to_graph_map_.end(); ++it) {
        // TODO: check if the extra copy is needed
        it->first->position() = ci::Vec2f(it->second->position());
    }
}

void GraphPhysics::setup_new_node(ParticlePtr p, ParticlePtr q,
    float length, float strength) {
    add_spacers_to_node(p_system_, p, q);
    make_edge_between(p_system_, p, q, length, strength);

    ci::Rand rand;
    rand.randomize();

    p->position() = q->position() + ci::Vec2f(rand.randFloat(-1.0f, 1.0f),
        rand.randFloat(-1.0f, 1.0f));
}

void GraphPhysics::add_spacers_to_node(ParticleSystemPtr system, 
    ParticlePtr p, ParticlePtr r) {
    for (std::vector<ParticlePtr>::iterator it = system->particles_.begin();
        it != system->particles_.end(); ++it) {

        ParticlePtr q = *it;

        if (p.get() != q.get() && p.get() != r.get())
            make_separation_between(system, p, q);
    }
}

void GraphPhysics::make_edge_between(ParticleSystemPtr system, ParticlePtr a, 
    ParticlePtr b) {
    system->make_spring(*(a.get()), *(b.get()), edge_strength_,
        edge_strength_, edge_length_);
}

void GraphPhysics::make_edge_between(ParticleSystemPtr system, ParticlePtr a, 
    ParticlePtr b, float length, float strength) {
    system->make_spring(*(a.get()), *(b.get()), strength, strength, length);
}

void GraphPhysics::make_separation_between(ParticleSystemPtr system, 
    ParticlePtr a, ParticlePtr b) {
    system->make_attraction(*(a.get()), *(b.get()), -spacer_strength_, 20);
}

ParticlePtr GraphPhysics::create_unlinked_particle(ParticlePtr q) {
    ParticlePtr p = p_system_->make_particle();

    add_spacers_to_node(p_system_, p, q);

    return p;
}

void GraphPhysics::link_nodes(GraphNodePtr a, GraphNodePtr b) {
    link_nodes(a, b, edge_length_, edge_strength_);
}

// this is the new main way particles are added to the system
// it is basically the new "setup new node"
void GraphPhysics::link_nodes(GraphNodePtr new_node, GraphNodePtr existing_node, 
    float length, float strength) {
    ParticlePtr new_ptr = new_node->particle();
    ParticlePtr existing_ptr = existing_node->particle();

    graph::Particle& new_p = *(new_ptr.get());
    graph::Particle& existing_p = *(existing_ptr.get());

    p_system_->make_spring(new_p, existing_p, strength, strength, length);

    // make sure there isn't a residual attraction
    p_system_->remove_attraction(new_p, existing_p);

    ci::Rand rand;
    rand.randomize();

    new_p.position() = existing_p.position() + ci::Vec2f(rand.randFloat(-1.0f, 1.0f),
        rand.randFloat(-1.0f, 1.0f));
}


GraphPhysics& GraphPhysics::instance() {
    if (instance_ == NULL) {
        instance_ = new GraphPhysics();
        instance_->init();
    }

    return *instance_;
}

void GraphPhysics::cleanup() {
    delete instance_;
}


}
