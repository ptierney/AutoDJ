
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "graph/graph_ParticleSystem.h"
#include "graph/graph_Particle.h"
#include "graph/graph_Spring.h"

#include "AdjApp.h"
#include "adj/adj_Visualizer.h"
#include "adj/adj_GraphPhysics.h"

namespace adj {

Visualizer::Visualizer() {
    scale_ = 1.0f;
    centroid_ = ci::Vec2i::zero();
    node_size_ = 10.0f;
}

Visualizer::~Visualizer() {
    AdjApp::instance().unregisterMouseDrag(mouse_drag_cb_id_);
    AdjApp::instance().unregisterKeyDown(key_cb_id_);
}

void Visualizer::setup() {
    mouse_drag_cb_id_ = AdjApp::instance().registerMouseDrag(
        this, &Visualizer::mouse_drag);
    key_cb_id_ = AdjApp::instance().registerKeyDown(this, 
        &Visualizer::key_down);

    glEnable(GL_LINE_SMOOTH);
    ci::gl::enableAlphaBlending();

    p_system_ = GraphPhysics::instance().particle_system();

    p_system_->make_particle();
}

void Visualizer::update() {
    GraphPhysics::instance().update();
}

void Visualizer::draw() {
    ci::gl::setMatricesWindow(AdjApp::instance().getWindowSize());
    ci::gl::clear(ci::Color::white());

    if (p_system_->particles_.size() > 1)
        update_centroid();

    ci::gl::color(ci::Color::black());

    ci::gl::translate(static_cast<ci::Vec2f>(AdjApp::instance().getWindowSize()) / 2.0f);
    ci::gl::scale(ci::Vec3f::one() * scale_);
    ci::gl::translate(centroid_);

    draw_network();
}

void Visualizer::shutdown() {
    GraphPhysics::cleanup();
}

void Visualizer::update_centroid() {
    if (p_system_->particles_.empty())
        return;

    ParticlePtr first = p_system_->particles_[0];

    float x_min = first->position().x;
    float x_max = first->position().x;
    float y_min = first->position().y;
    float y_max = first->position().y;

    for (std::vector<ParticlePtr>::iterator it = p_system_->particles_.begin();
        it != p_system_->particles_.end(); ++it) {

        graph::Particle& p = *((*it).get());

        x_max = ci::math<float>::max(x_max, p.position().x);
        x_min = ci::math<float>::min(x_min, p.position().x);
        y_min = ci::math<float>::min(y_min, p.position().y);
        y_max = ci::math<float>::max(y_max, p.position().y);
    }

    float delta_x = x_max - x_min;
    float delta_y = y_max - y_min;

    centroid_.x = x_min + 0.5f * delta_x;
    centroid_.y = y_min + 0.5f * delta_y;

    if (delta_y > delta_x)
        scale_ = AdjApp::instance().getWindowHeight() / (delta_y + 50.0f);
    else
        scale_ = AdjApp::instance().getWindowWidth() / (delta_x + 50.0f);
}

void Visualizer::draw_network() {
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

void Visualizer::add_node() {
    GraphPhysics::instance().create_node();
}

bool Visualizer::mouse_drag(ci::app::MouseEvent) {
    add_node();

    return true;
}

bool Visualizer::key_down(ci::app::KeyEvent) {
    add_node();

    return true;
}

}
