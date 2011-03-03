
#include <algorithm>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>

#include <cinder/Vector.h>

#include <graph/graph_Particle.h>
#include <graph/graph_Spring.h>

namespace graph {

Particle::Particle(float m) { 
    reset();

    mass_ = m;
    fixed_ = false;

    id_ = get_random_id();
}

ParticleId Particle::get_random_id() {
    boost::uuids::random_generator gen;
    return boost::lexical_cast<std::string>(gen());
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

void Particle::add_spring(SpringPtr s) {
    // TODO: decide if this is needed
    /*
    if (std::find(springs_.begin(), springs_.end(), s) !=
        springs_.end())
        return;
    */

    springs_.push_back(s);
}

}
