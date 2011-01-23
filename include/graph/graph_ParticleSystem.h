
#pragma once

#include <vector>

#include "adj/adj_Adj.h"

#include "cinder/Vector.h"

namespace graph {

class Integrator;
class Particle;
class Spring;
class Attraction;

typedef std::shared_ptr<Particle> ParticlePtr;

class ParticleSystem {
public:
    ParticleSystem();
    ParticleSystem(ci::Vec2f gravity, float drag);

    void init();

    ParticlePtr make_particle();
    ParticlePtr make_particle(float mass, ci::Vec2f pos);
    std::shared_ptr<Spring> make_spring(Particle& a, Particle& b,
        float ks, float d, float r);
    std::shared_ptr<Attraction> make_attraction(Particle& a, Particle& b,
        float k, float min_distance);
    void remove_attraction(Particle& a, Particle& b);

    void clear();

    void apply_forces();
    void clear_forces();

    void tick();
    void tick(float t);

    std::vector<ParticlePtr> particles_;
    std::vector<std::shared_ptr<Spring> > springs_;
    std::vector<std::shared_ptr<Attraction> > attractions_;

private:
    std::shared_ptr<Integrator> integrator_;

    ci::Vec2f gravity_;
    float drag_;
};

}