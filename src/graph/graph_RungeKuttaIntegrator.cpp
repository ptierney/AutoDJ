
#include "graph/graph_RungeKuttaIntegrator.h"
#include "graph/graph_ParticleSystem.h"
#include "graph/graph_Particle.h"

namespace graph {

RungeKuttaIntegrator::RungeKuttaIntegrator(ParticleSystem& s) 
    : s_(s) {

}

void RungeKuttaIntegrator::allocate_particles() {
    while (s_.particles_.size() > original_positions_.size()) {
        original_positions_.push_back(ci::Vec2f::zero());
        original_velocities_.push_back(ci::Vec2f::zero());
        k1_forces_.push_back(ci::Vec2f::zero());
        k1_velocities_.push_back(ci::Vec2f::zero());
        k2_forces_.push_back(ci::Vec2f::zero());
        k2_velocities_.push_back(ci::Vec2f::zero());
        k3_forces_.push_back(ci::Vec2f::zero());
        k3_velocities_.push_back(ci::Vec2f::zero());
        k4_forces_.push_back(ci::Vec2f::zero());
        k4_velocities_.push_back(ci::Vec2f::zero());
    }
}

void RungeKuttaIntegrator::step(float delta_t) {
    allocate_particles();

    std::shared_ptr<Particle> p;

    // save original position and velocities
    int i = 0;
    for (particles_it_ = s_.particles_.begin(), i = 0; particles_it_ != 
        s_.particles_.end(); ++particles_it_, ++i) {

        Particle& p = *(*particles_it_);

        if (p.is_free()) {
            original_positions_[i] = p.position();
            original_velocities_[i] = p.velocity();
        }

        p.force() = ci::Vec2f::zero(); // and clear the forces
    }

    // get all the k1 values
    s_.apply_forces();

    // save the intermediate forces

    for (particles_it_ = s_.particles_.begin(), i = 0; particles_it_ != 
        s_.particles_.end(); ++particles_it_, ++i) {

        Particle& p = *(*particles_it_);

        if (p.is_free()) {
            k1_forces_[i] = p.force();
            k1_velocities_[i] = p.velocity();
        }

        p.force() = ci::Vec2f::zero(); // and clear the forces
    }

    for (particles_it_ = s_.particles_.begin(), i = 0; particles_it_ != 
        s_.particles_.end(); ++particles_it_, ++i) {

        Particle& p = *(*particles_it_);

        if (p.is_fixed())
            continue;

        p.position() = original_positions_[i] + 
            k1_velocities_[i] * 0.5f * delta_t;

        p.velocity() = original_velocities_[i] + 
            k1_forces_[i] * 0.5f * delta_t / p.mass_;
    }

    s_.apply_forces();

    // save the intermediate forces
    
    for (particles_it_ = s_.particles_.begin(), i = 0; particles_it_ != 
        s_.particles_.end(); ++particles_it_, ++i) {

        Particle& p = *(*particles_it_);

        if (p.is_free()) {
            k2_forces_[i] = p.force();
            k2_velocities_[i] = p.velocity();
        }

        p.force() = ci::Vec2f::zero();
    }

    // get k3 values
    
    for (particles_it_ = s_.particles_.begin(), i = 0; particles_it_ !=
        s_.particles_.end(); ++particles_it_, ++i) {

        Particle& p = *(*particles_it_);

        if (p.is_fixed())
            continue;

        p.position() = original_positions_[i] + 
            k2_velocities_[i] * 0.5f * delta_t;

        p.velocity() = original_velocities_[i] + 
            k2_forces_[i] * 0.5f * delta_t / p.mass_;
    }

    s_.apply_forces();

    // save the intermediate forces
    
    for (particles_it_ = s_.particles_.begin(), i = 0; particles_it_ != 
        s_.particles_.end(); ++particles_it_, ++i) {

        Particle& p = *(*particles_it_);

        if (p.is_free()) {
            k3_forces_[i] = p.force();
            k3_velocities_[i] = p.velocity();
        }

        p.force() = ci::Vec2f::zero();
    }

    // get k4 values

    for (particles_it_ = s_.particles_.begin(), i = 0; particles_it_ != 
        s_.particles_.end(); ++particles_it_, ++i) {

        Particle& p = *(*particles_it_);

        if (p.is_fixed())
            continue;

        p.position() = original_positions_[i] + k3_velocities_[i] * delta_t;
        p.velocity() = original_velocities_[i] + k3_forces_[i] * delta_t / p.mass_;
    }

    s_.apply_forces();

    // save the intermediate forces

    for (particles_it_ = s_.particles_.begin(), i = 0; particles_it_ !=
        s_.particles_.end(); ++particles_it_, ++i) {

        Particle& p = *(*particles_it_);

        if (p.is_fixed())
            continue;

        k4_forces_[i] = p.force();
        k4_velocities_[i] = p.velocity();
    }

    // put them all together

    for (particles_it_ = s_.particles_.begin(), i = 0; particles_it_ != 
        s_.particles_.end(); ++particles_it_, ++i) {

        Particle& p = *(*particles_it_);

        p.age() += delta_t;

        if (p.is_fixed())
            continue;

        // update position

        p.position() = original_positions_[i] + delta_t / 6.0f * 
            (k1_velocities_[i] + 2.0f*k2_velocities_[i] + 2.0f*k3_velocities_[i] +
            k4_velocities_[i]);

        p.velocity() = original_velocities_[i] + delta_t / (6.0f * p.mass_) *
            (k1_forces_[i] + 2.0f*k2_forces_[i] + 2.0f*k3_forces_[i] + 
            k4_forces_[i]);
    }
}

}