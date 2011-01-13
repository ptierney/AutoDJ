
#pragma once

#include "graph/graph_Force.h"

namespace graph {

class Particle;

class Attraction : public Force {
public:
    Attraction(Particle& a, Particle& b, float k, float distance_min);

    float get_minimum_distance() { return distance_min_; }
    void set_minimum_distance(float d) { distance_min_ = d; }

    void turn_off() { on_ = false; }
    void turn_on() { on_ = true; }
    bool is_on() { return on_; }
    bool is_off() { return !on_; }

    float get_strength() { return k_; }
    void set_strength(float k) { k_ = k; }

    const Particle& get_one_end() const { return a_; }
    Particle& get_one_end() { return a_; }
    const Particle& get_the_other_end() const { return b_; }
    Particle& get_the_other_end() { return b_; }

    void apply();

private:
    Particle& a_;
    Particle& b_;

    float k_;
    bool on_;
    float distance_min_;
    float distance_min_squared_;
};

}


