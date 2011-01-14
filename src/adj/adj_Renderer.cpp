
#include "cinder/gl/gl.h"

#include "graph/graph_Particle.h"
#include "graph/graph_ParticleSystem.h"
#include "graph/graph_Spring.h"

#include "AdjApp.h"
#include "adj/adj_Renderer.h"
#include "adj/adj_Camera.h"
#include "adj/adj_GraphPhysics.h"

namespace adj {

Renderer::Renderer() {
    node_size_ = 10.0f;
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
    ci::gl::clear(ci::Color::white());

    Camera::instance().transform_draw();

    draw_nodes();
}

void Renderer::draw_nodes() {
    ci::gl::color(ci::Color(0.4f, 0.4f, 0.4f));

    for (std::vector<ParticlePtr>::iterator it = p_system_->particles_.begin();
        it != p_system_->particles_.end(); ++it) {

        ci::gl::drawSolidCircle((*it)->position(), node_size_);
    }

    ci::gl::color(ci::Color::black());
    glLineWidth(3.0f);

    glBegin(GL_LINES);

    for (std::vector<std::tr1::shared_ptr<graph::Spring> >::iterator it = 
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