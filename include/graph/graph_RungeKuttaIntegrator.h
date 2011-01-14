
#pragma once

#include <vector>

#include "cinder/Cinder.h"

#if defined (CINDER_MAC)
    #include <tr1/memory>
#endif

#include "cinder/Vector.h"

#include "graph/graph_Integrator.h"

namespace graph {

class Particle;
class ParticleSystem;

class RungeKuttaIntegrator : public Integrator {
public:
    RungeKuttaIntegrator(ParticleSystem& s);

    void step(float);

private:
    void allocate_particles();

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