
#include "graph/graph_ParticleSystem.h"

#include "AdjApp.h"
#include "adj/adj_Visualizer.h"
#include "adj/adj_GraphPhysics.h"
#include "adj/adj_Renderer.h"
#include "adj/adj_Camera.h"
#include "adj/adj_GraphNodeFactory.h"

namespace adj {

Visualizer::Visualizer() {
    // nothing here
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

    Renderer::instance().setup();
    Camera::instance().setup();

    GraphPhysics::instance().particle_system()->make_particle();
}

void Visualizer::update() {
    GraphPhysics::instance().update();
    Camera::instance().update(); // updates centroid
}

void Visualizer::draw() {
    Renderer::instance().draw();
}

void Visualizer::shutdown() {
    GraphPhysics::cleanup();
    Camera::cleanup();
    Renderer::cleanup();
}

void Visualizer::add_node() {
    GraphNodeFactory::instance().add_empty_node();
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
