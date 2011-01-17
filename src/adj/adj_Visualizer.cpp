
#include "graph/graph_ParticleSystem.h"

#include "AdjApp.h"
#include "adj/adj_Visualizer.h"
#include "adj/adj_GraphPhysics.h"
#include "adj/adj_Renderer.h"
#include "adj/adj_Camera.h"
#include "adj/adj_GraphNodeFactory.h"
#include "adj/adj_Song.h"
#include "adj/adj_PlayManager.h"

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

    PlayManager::instance().setup();
    
    try {
        // load database from file, create song objects
        SongFactory::instance().load_song_database();
    } catch (...) {
        ci::app::console() << "Could not find database file, exiting." << std::endl;
        exit;
    }

    GraphNodeFactory::instance().add_to_random_node();
}

void Visualizer::update() {
    GraphPhysics::instance().update();
    Camera::instance().update(); // updates centroid
}

void Visualizer::draw() {
    Renderer::instance().draw();
}

void Visualizer::shutdown() {
    GraphNodeFactory::cleanup();
    SongFactory::cleanup();
    GraphPhysics::cleanup();
    Camera::cleanup();
    Renderer::cleanup();
}

void Visualizer::add_node() {
    GraphNodeFactory::instance().add_to_random_node();
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
