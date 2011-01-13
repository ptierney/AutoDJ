
#include "cinder/Cinder.h"

#if defined (CINDER_MAC)
    #include <tr1/memory>
#endif

#include "graph/graph_ParticleSystem.h"
#include "graph/graph_RungeKuttaIntegrator.h"
#include "graph/graph_Particle.h"
#include "graph/graph_Spring.h"
#include "graph/graph_Attraction.h"
#include "graph/graph_Integrator.h"

namespace graph {

ParticleSystem::ParticleSystem() {
    integrator_ = std::tr1::shared_ptr<RungeKuttaIntegrator>(
        new RungeKuttaIntegrator(*this));

    gravity_ = ci::Vec2f::zero();
    drag_ = 0.001f;
}

std::tr1::shared_ptr<Particle> ParticleSystem::make_particle(float mass, 
    ci::Vec2f pos) {
    std::tr1::shared_ptr<Particle> p(new Particle(mass));
    p->position() = pos;
    particles_.push_back(p);
    return p;
}

std::tr1::shared_ptr<Spring> ParticleSystem::make_spring(Particle& a, Particle& b,
    float ks, float d, float r) {

    std::tr1::shared_ptr<Spring> s(new Spring(a, b, ks, d, r));
    springs_.push_back(s);
    return s;
}

void ParticleSystem::apply_forces() {
    if (gravity_ != ci::Vec2f::zero()) {
        for (std::vector<std::tr1::shared_ptr<Particle> >::iterator it = 
            particles_.begin(); it != particles_.end(); ++it) {
            (*it)->force() += gravity_;
        }
    }

    for (std::vector<std::tr1::shared_ptr<Particle> >::iterator it = 
        particles_.begin(); it != particles_.end(); ++it) {
        (*it)->force() += (*it)->velocity() * -drag_;
    }

    for (std::vector<std::tr1::shared_ptr<Spring> >::iterator it =
        springs_.begin(); it != springs_.end(); ++it) {
        (*it)->apply();
    }

    for (std::vector<std::tr1::shared_ptr<Attraction> >::iterator it = 
        attractions_.begin(); it != attractions_.end(); ++it) {
        (*it)->apply();        
    }

    // traer does custom forces here
}

void ParticleSystem::clear_forces() {
    for (std::vector<std::tr1::shared_ptr<Particle> >::iterator it = 
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

}
