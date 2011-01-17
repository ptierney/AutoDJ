
#include <deque>

#include "cinder/gl/gl.h"
#include "cinder/Color.h"

#include "graph/graph_Particle.h"
#include "graph/graph_ParticleSystem.h"
#include "graph/graph_Spring.h"

#include "AdjApp.h"
#include "adj/adj_Renderer.h"
#include "adj/adj_Camera.h"
#include "adj/adj_GraphPhysics.h"
#include "adj/adj_GraphNodeFactory.h"
#include "adj/adj_GraphNode.h"

namespace adj {

Renderer::Renderer() {
    node_size_ = 10.0f;
    background_color_ = ci::Color::black();
    network_color_ = ci::ColorA(1.0f, 1.0f, 1.0f, 0.25f);
}

void Renderer::init() {
    p_system_ = GraphPhysics::instance().particle_system();
}

void Renderer::setup() {
    glEnable(GL_LINE_SMOOTH);
    ci::gl::enableAlphaBlending();
}

void Renderer::draw() {
    ci::gl::setMatricesWindow(AdjApp::instance().getWindowSize());
    ci::gl::clear(background_color_);

    Camera::instance().transform_draw();

    draw_nodes();
    draw_connections();
}

void Renderer::draw_nodes() {
    for (std::vector<GraphNodePtr>::iterator it = GraphNodeFactory::instance().nodes().begin();
        it != GraphNodeFactory::instance().nodes().end(); ++it) {

        (*it)->draw();
    }
}

// in the future, the nodes will probably draw their own connections, or at least
// store their line weight based on their distance to "now playing"
void Renderer::draw_connections() {
    ci::gl::color(network_color_);
    glLineWidth(3.0f);

    glBegin(GL_LINES);

    for (std::vector<std::shared_ptr<graph::Spring> >::iterator it = 
        p_system_->springs_.begin(); it != p_system_->springs_.end(); ++it)  {

        graph::Particle& a = (*it)->get_one_end();
        graph::Particle& b = (*it)->get_the_other_end();

        glVertex2f(a.position());
        glVertex2f(b.position());
    }

    glEnd();
}


Renderer* Renderer::instance_ = NULL;

Renderer& Renderer::instance() {
    if (instance_ == NULL) {
        instance_ = new Renderer();
        instance_->init();
    }

    return *instance_;
}

void Renderer::cleanup() {
    if (instance_ == NULL)
        return;

    delete instance_;
}

}