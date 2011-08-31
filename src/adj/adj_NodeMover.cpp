

#include <vector>

#include <adj/adj_Adj.h>

#include <cinder/BSpline.h>
#include <cinder/app/App.h>

#include <graph/graph_Particle.h>

#include <adj/adj_GraphNode.h>
#include <adj/adj_GraphNodeFactory.h>
#include <adj/adj_GraphPhysics.h>
#include <adj/adj_NodeMover.h>

namespace adj {

NodeMover* NodeMover::instance_ = NULL;

NodeMover::NodeMover() {
    rotate_clockwise_ = true;
    rotation_speed_ = 0.2;
    contraction_scale_ = 1.0f;
    num_contraction_cycles_ = 5;
}

void NodeMover::init() {

}

void NodeMover::update() {
    std::vector<GraphNodePtr>& nodes = GraphNodeFactory::instance().nodes();

    GraphNodePtr center_node = PlayManager::instance().now_playing();

    if (!center_node)
        return;

    ci::Vec2f& cp = center_node->particle()->position();

    for (std::vector<GraphNodePtr>::iterator it = nodes.begin(); it != 
        nodes.end(); ++it) {

        if (*it == center_node)
            continue;

        ci::Vec2f& p = (*it)->particle()->position();
        
        ci::Vec3f right3d = ci::Vec3f(p.x, p.y, 0.0f).cross(ci::Vec3f::zAxis());

        ci::Vec2f force(right3d.x, right3d.y);
        
        bool is_right_of_center = p.x > cp.x;
        bool is_below_center = p.y > cp.y;
        bool is_pointing_down = force.y > 0.0f;
        bool is_pointing_right = force.x > 0.f;

        if (rotate_clockwise_) {
            if ((is_right_of_center && !is_pointing_down) ||
                (!is_right_of_center && is_pointing_down))
                force.y *= -1.f;
            if ((is_below_center && is_pointing_right) ||
                (!is_below_center && !is_pointing_right))
                force.x *= -1.f;
        } else {
            if ((is_right_of_center && is_pointing_down) ||
                (!is_right_of_center && !is_pointing_down))
                force.y *= -1.f;
            if ((is_below_center && !is_pointing_right) ||
                (!is_below_center && is_pointing_right))
                force.x *= -1.f;
        }

        force.normalize();
        force *= rotation_speed_;

        float contraction_amount = ci::math<float>::cos(num_contraction_cycles_ 
            * get_rotation_angle(cp, p));

        force += (p - cp).normalized() * contraction_amount * contraction_scale_;

        (*it)->particle()->position() += force;
    }
}

float NodeMover::get_rotation_angle(const ci::Vec2f& center, 
    const ci::Vec2f& node) {
    ci::Vec2f dir = node - center;
    float ang = ci::math<float>::atan2(dir.x, dir.y);

    return ang;
}

ci::Vec2f NodeMover::get_modulate_point(float t, const ci::Vec2f& center) {
    float radius = GraphPhysics::instance().edge_length();
    float wave_height = radius  / 5;
    int num_cycles = 5;

    float r = radius - wave_height * ci::math<float>::cos(num_cycles * t);

    float x = r * ci::math<float>::cos(t);
    float y = r * ci::math<float>::sin(t);

    return ci::Vec2f(x, y) + center;
}

void NodeMover::draw() {

}

NodeMover& NodeMover::instance() {
    if (instance_ == NULL) {
        instance_ = new NodeMover();
        instance_->init();
    }

    return *instance_;
}

void NodeMover::cleanup() {
    if (instance_ == NULL)
        return;

    delete instance_;
}

}