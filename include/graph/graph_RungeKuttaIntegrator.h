
#pragma once

#include <vector>

#include <cinder/Vector.h>

#include "graph/graph_Integrator.h"

namespace graph {

class Particle;
class ParticleSystem;

class RungeKuttaIntegrator : public Integrator {
public:
    RungeKuttaIntegrator(ParticleSystem& s);

    void allocate_particles();

    void step(float);

private:
    std::vector<ci::Vec2f> original_positions_;
    std::vector<ci::Vec2f> original_velocities_;
    std::vector<ci::Vec2f> k1_forces_;
    std::vector<ci::Vec2f> k1_velocities_;
    std::vector<ci::Vec2f> k2_forces_;
    std::vector<ci::Vec2f> k2_velocities_;
    std::vector<ci::Vec2f> k3_forces_;
    std::vector<ci::Vec2f> k3_velocities_;
    std::vector<ci::Vec2f> k4_forces_;
    std::vector<ci::Vec2f> k4_velocities_;

    ParticleSystem& s_;

    std::vector<std::tr1::shared_ptr<Particle> >::iterator particles_it_;
};

}