
#include "cinder/Vector.h"

#include "graph/graph_Particle.h"

namespace graph {

Particle::Particle(float m) { 
    reset();

    mass_ = m;
    fixed_ = false;
}

float Particle::distance_to(const Particle& p) {
    return position_.distance(p.position());
}

void Particle::make_fixed() {
    fixed_ = true;
    velocity_ = ci::Vec2f::zero();
}

void Particle::make_free() {
    fixed_ = false;
}

void Particle::reset() {
    age_ = 0;
    dead_ = false;

    position_ = ci::Vec2f::zero();
    velocity_ = ci::Vec2f::zero();
    force_ = ci::Vec2f::zero();
}

}
