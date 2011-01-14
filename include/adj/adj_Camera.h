
#pragma once

#include "cinder/Vector.h"

namespace graph {
    class ParticleSystem;
}

namespace adj {

class GraphNode;
typedef std::tr1::shared_ptr<graph::ParticleSystem> ParticleSystemPtr;

// main program must call update() and transform_draw() each draw loop
// otherwise will center_on_node, etc. and camera 

class Camera {
public:
    static Camera& instance();
    static void cleanup();

    void setup();
    void update(); // does time stuff for easing
    // does all necessary translations and scalings
    void transform_draw();

    // centers on a node fairly tight
    void center_on_node(const GraphNode&);
    // frame the square created by a and b
    void frame_nodes(const GraphNode& a, const GraphNode& b);
    void frame_network(); // frames entire network

private:
    Camera();
    void init();

    void update_centroid();

    static Camera* instance_;

    float width_border_;
    float height_border_;

    float scale_;
    ci::Vec2f centroid_;

    ParticleSystemPtr p_system_;
};

}
