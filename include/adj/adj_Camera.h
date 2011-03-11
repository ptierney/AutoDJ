
#pragma once

#include "cinder/Vector.h"

namespace graph {
    class ParticleSystem;
}

namespace adj {

class GraphNode;
typedef std::shared_ptr<graph::ParticleSystem> ParticleSystemPtr;

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
    
    void get_min_max_over_all(float& x_min, float& x_max,
        float& y_min, float& y_max);
    void get_min_max_over_active(float& x_min, float& x_max,
        float& y_min, float& y_max);
    
    static Camera* instance_;

    float width_border_;
    float height_border_;

    float scale_;
    ci::Vec2f centroid_;

    int node_crop_cuttoff_;

    float scale_damping_;
    float centroid_damping_;

    ParticleSystemPtr p_system_;
    ParticleSystemPtr box_p_system_;
};

}
