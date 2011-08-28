
#pragma once

#include <cinder/BSpline.h>

namespace adj {

class NodeMover {
public:
    static NodeMover& instance();
    static void cleanup();

    // for debug purposes
    void draw();
    void update();

    bool& rotate_clockwise() { return rotate_clockwise_; }

    float get_rotation_angle(const ci::Vec2f& center, const ci::Vec2f& node);
    ci::Vec2f get_modulate_point(float angle, const ci::Vec2f& center);

private:
    NodeMover();
    void init();

    bool rotate_clockwise_;
    float rotation_speed_;

    float contraction_scale_;
    int num_contraction_cycles_;

    static NodeMover* instance_;
};

}