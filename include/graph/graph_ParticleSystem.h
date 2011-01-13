
#pragma once

#include <vector>

#include "cinder/Cinder.h"

#if defined (CINDER_MAC)
    #include <tr1/memory>
#endif

namespace graph {

class Integrator;

class ParticleSystem {
public:

private:
    std::tr1::shared_ptr<Integrator> integrator_;

    std::vector<Particle> particles_;
    std::vector<Spring> springs_;


};

}