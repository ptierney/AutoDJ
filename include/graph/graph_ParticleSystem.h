
#pragma once

#include <vector>

#include "cinder/Cinder.h"

#if defined (CINDER_MAC)
    #include <tr1/memory>
#endif

namespace graph {

class Integrator;
class Particle;
class Spring;

class ParticleSystem {
public:

private:
    std::tr1::shared_ptr<Integrator> integrator_;

    std::vector<std::tr1::shared_ptr<Particle> > particles_;
    std::vector<std::tr1::shared_ptr<Spring> > springs_;

    

};

}