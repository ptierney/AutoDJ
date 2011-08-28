
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

private:
    NodeMover();
    void init();

    ci::BSpline2f track_;
    std::vector<ci::Vec2f> track_points_;

    static NodeMover* instance_;
};

}