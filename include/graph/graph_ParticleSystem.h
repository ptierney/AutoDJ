
#pragma once

#include <vector>

#include "cinder/Cinder.h"

#if defined (CINDER_MAC)
    #include <tr1/memory>
#endif

#include "cinder/Vector.h"

namespace graph {

class Integrator;
class Particle;
class Spring;
class Attraction;

class ParticleSystem {
public:
    ParticleSystem();

    std::tr1::shared_ptr<Particle> make_particle(float mass, ci::Vec2f pos);
    std::tr1::shared_ptr<Spring> make_spring(Particle& a, Particle& b,
        float ks, float d, float r);

    void apply_forces();
    void clear_forces();

    std::vector<std::tr1::shared_ptr<Particle> > particles_;
    std::vector<std::tr1::shared_ptr<Spring> > springs_;
    std::vector<std::tr1::shared_ptr<Attraction> > attractions_;

private:
    std::tr1::shared_ptr<Integrator> integrator_;

    ci::Vec2f gravity_;
    float drag_;
};

}