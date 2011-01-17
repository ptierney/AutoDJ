
#include "cinder/gl/gl.h"

#include "graph/graph_Particle.h"

#include "adj/adj_GraphNode.h"

namespace adj {

GraphNode::GraphNode() {
    is_valid_ = false;
    size_ = 10.0f;
}

void GraphNode::draw() {
    ci::gl::drawSolidCircle(particle_->position(), size_);
}

void GraphNode::add_child(GraphNodePtr child) {
    // check children_ to see if it already contains child
    // TODO: replace this with a proper std::algorithm
    for (std::vector<GraphNodePtr>::const_iterator it = children_.begin();
        it != children_.end(); ++it) {
        if (it->get() == child.get())
            return;
    }

    children_.push_back(child);
}

}
