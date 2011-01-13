
#pragma once

namespace graph {

class Force {
public:
    virtual ~Force() { }

    virtual void turn_on() = 0;
    virtual void turn_off() = 0;
    virtual bool is_on() = 0;
    virtual bool is_off() = 0;
    virtual void apply() = 0;
};

}