
#pragma once

#include <vector>

#include <cinder/Vector.h>

#include "graph/graph_Integrator.h"

namespace graph {

class RungeKuttaIntegrator : public Integrator {
public:


private:
    std::vector<ci::Vec2f> original_positions_;
    std::vector<ci::Vec2f> original_velocities_;
    std::vector<ci::Vec2f> k1_forces_;
    std::vector<ci::Vec2f> k1_velocities_;
    std::vector<ci::Vec2f> k2_forces_;
    std::vector<ci::Vec2f> k2_velocities_;
    std::vector<ci::Vec2f> k3_forces_;
    std::vector<ci::Vec2f> k4_forces_;
    std::vector<ci::Vec2f> k4_velocities_;

    ParticleSystem s_;
};

}