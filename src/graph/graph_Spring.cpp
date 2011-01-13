
#include "cinder/CinderMath.h"

#include "graph/graph_Spring.h" 
#include "graph/graph_Particle.h"

namespace graph {

Spring::Spring(Particle& a, Particle& b, float ks, float d, float r)
    : a_(a), b_(b), spring_constant_(ks), damping_(d), rest_length_(r) {
    // nothing here
}

Spring::~Spring() {
    // nothing here
}

float Spring::current_length() {
    a_.position().distance(b_.position());
}

void Spring::apply() {
    if (on_ && (a_.is_free() || b_.is_free())) {
        
        ci::Vec2f a_to_b = a_.position() - b_.position();
        float a_to_b_dist = a_to_b.length();

        // TODO: replace with better check for zero
        if (a_to_b_dist <= EPSILON && a_to_b_dist >= -EPSILON) {
            a_to_b = ci::Vec2f::zero();
        } else {
            a_to_b.normalize();
        }

        float spring_force = -(a_to_b_dist - rest_length_) * spring_constant_;

        ci::Vec2f v_a_to_b = a_.velocity() - b_.velocity();

        float damping_force = -damping_ * (a_to_b.x * v_a_to_b.x + 
            a_to_b.y * v_a_to_b.y);

        float r = spring_force + damping_force;

        a_to_b *= r;

        if (a_.is_free())
            a_.force() += a_to_b;

        if (b_.is_free())
            b_.force() -= a_to_b;
    }
}



}