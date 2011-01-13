
#pragma once

namespace graph {

class Integrator {
public:
    virtual ~Integrator() { }

    virtual void step(float) = 0;
};

}