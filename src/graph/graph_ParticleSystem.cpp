
#include "adj/adj_Adj.h"

#include "graph/graph_ParticleSystem.h"
#include "graph/graph_RungeKuttaIntegrator.h"
#include "graph/graph_Particle.h"
#include "graph/graph_Spring.h"
#include "graph/graph_Attraction.h"
#include "graph/graph_Integrator.h"

namespace graph {

ParticleSystem::ParticleSystem() {
    gravity_ = ci::Vec2f::zero();
    drag_ = 0.001f;
}

ParticleSystem::ParticleSystem(ci::Vec2f gravity, float somedrag) {
    gravity_ = gravity;
    drag_ = somedrag;
}

void ParticleSystem::init() {
    integrator_ = std::shared_ptr<RungeKuttaIntegrator>(
        new RungeKuttaIntegrator(*this));
}

std::shared_ptr<Particle> ParticleSystem::make_particle() {
    return make_particle(1.0f, ci::Vec2f::zero());
}

std::shared_ptr<Particle> ParticleSystem::make_particle(float mass, 
    ci::Vec2f pos) {
    std::shared_ptr<Particle> p(new Particle(mass));
    p->position() = pos;
    particles_.push_back(p);
    return p;
}

std::shared_ptr<Spring> ParticleSystem::make_spring(Particle& a, Particle& b,
    float ks, float d, float r) {

    std::shared_ptr<Spring> s(new Spring(a, b, ks, d, r));
    springs_.push_back(s);
    return s;
}

std::shared_ptr<Attraction> ParticleSystem::make_attraction(Particle& a, 
    Particle& b, float k, float min_distance) {
    std::shared_ptr<Attraction> m(new Attraction(a, b, k, min_distance));
    attractions_.push_back(m);
    return m;
}

void ParticleSystem::apply_forces() {
    if (gravity_ != ci::Vec2f::zero()) {
        for (std::vector<std::shared_ptr<Particle> >::iterator it = 
            particles_.begin(); it != particles_.end(); ++it) {
            (*it)->force() += gravity_;
        }
    }

    for (std::vector<std::shared_ptr<Particle> >::iterator it = 
        particles_.begin(); it != particles_.end(); ++it) {
        (*it)->force() += (*it)->velocity() * -drag_;
    }

    for (std::vector<std::shared_ptr<Spring> >::iterator it =
        springs_.begin(); it != springs_.end(); ++it) {
        (*it)->apply();
    }

    for (std::vector<std::shared_ptr<Attraction> >::iterator it = 
        attractions_.begin(); it != attractions_.end(); ++it) {
        (*it)->apply();        
    }

    // traer does custom forces here
}

void ParticleSystem::clear_forces() {
    for (std::vector<std::shared_ptr<Particle> >::iterator it = 
        particles_.begin(); it != particles_.end(); ++it) {
        (*it)->force() = ci::Vec2f::zero();
    }
}

void ParticleSystem::tick() {
    tick(1.0f);
}

void ParticleSystem::tick(float t) {
    integrator_->step(t);
}

void ParticleSystem::clear() {
    particles_.clear();
    springs_.clear();
    attractions_.clear();
}

}
