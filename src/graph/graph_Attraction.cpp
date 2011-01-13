
#include "cinder/Vector.h"
#include "cinder/CinderMath.h"

#include "graph/graph_Attraction.h"
#include "graph/graph_Particle.h"

namespace graph {

Attraction::Attraction(Particle& a, Particle& b, float k, float distance_min)
    : a_(a), b_(b), k_(k), distance_min_(distance_min) {
    on_ = true;
    distance_min_squared_ = distance_min_ * distance_min_;
}

void Attraction::apply() {
    if (on_ && (a_.is_free() || b_.is_free())) {
        ci::Vec2f a_to_b = a_.position() - b_.position();

        float a_to_b_dist_squared = a_to_b.lengthSquared();

        if (a_to_b_dist_squared < distance_min_squared_)
            a_to_b_dist_squared = distance_min_squared_;

        float force = k_ * a_.mass_ * b_.mass_ / a_to_b_dist_squared;

        // maybe check for 0 ?
        float length = ci::math<float>::sqrt(a_to_b_dist_squared);

        // normalize, save a square root
        a_to_b /= length;

        // multiply by force

        a_to_b *= force;

        // apply

        if (a_.is_free())
            a_.force() -= a_to_b;
        if (b_.is_free())
            b_.force() += a_to_b;
    }
}

}