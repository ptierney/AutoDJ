
#pragma once

#include "graph/graph_Force.h"

namespace graph {

class Particle;

class Spring : public Force {
public:
    Spring(Particle& a, Particle& b, float ks, float d, float r);
    virtual ~Spring();

    void turn_off() { on_ = false; }
    void turn_on() { on_ = true; }
    bool is_on() { return on_; }
    bool is_off() { return !on_; }

    Particle& get_one_end() { return a_; }
    Particle& get_the_other_end() { return b_; }

    float current_length();
    float rest_length() { return rest_length_; }
    void set_rest_length(float r) { rest_length_ = r; }
    float strength() { return spring_constant_; }
    void set_strength(float ks);
    float damping() { return damping_; }
    void set_damping(float d) { damping_ = d; }
    
    void apply();

private:
    float spring_constant_;
    float damping_;
    float rest_length_;
    bool on_;
    
    Particle& a_;
    Particle& b_;
};

}