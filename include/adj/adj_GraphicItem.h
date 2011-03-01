
#pragma once

namespace adj {

class GraphicItem {
public:
    GraphicItem();

    virtual float get_pos_x() = 0;
    virtual float get_pos_y() = 0;
    virtual float get_width() = 0;
    virtual float get_height() = 0;
    virtual void draw() = 0;
};

}