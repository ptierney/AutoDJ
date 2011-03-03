
#pragma once

#include <string>

namespace adj {

class GraphicItem {
public:
    GraphicItem();
    virtual ~GraphicItem();

    virtual float get_pos_x() = 0;
    virtual float get_pos_y() = 0;
    virtual float get_width() = 0;
    virtual float get_height() = 0;
    virtual void draw() = 0;
    virtual bool visible() = 0;
    virtual std::string type() = 0;
    // type return string?
};

}