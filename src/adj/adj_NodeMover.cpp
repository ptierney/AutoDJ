

#include <vector>

#include <cinder/BSpline.h>

#include <adj/adj_GraphNode.h>
#include <adj/adj_GraphNodeFactory.h>
#include <adj/adj_NodeMover.h>

namespace adj {

NodeMover* NodeMover::instance_ = NULL;

NodeMover::NodeMover() {

}

void NodeMover::init() {

}

void NodeMover::draw() {
    return;

    if (track_points_.empty())
        return;

    ci::gl::color(ci::Color::black());

    ci::Path2d p(track_);
    for (int i = 1; i < p.getNumPoints(); ++i) {
        ci::gl::drawLine(p.getPoint(i - 1), p.getPoint(i));
    }
}

void NodeMover::update() {
    std::vector<GraphNodePtr>& nodes = GraphNodeFactory::instance().nodes();

    for (std::vector<GraphNodePtr>::iterator it = nodes.begin(); it != 
        nodes.end(); ++it) {

        track_points_.push_back(ci::Vec2f((*it)->get_pos_x(), (*it)->get_pos_y()));
    }

    if (track_points_.empty())
        return;

    track_ = ci::BSpline2f(track_points_, 3, true, true);
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