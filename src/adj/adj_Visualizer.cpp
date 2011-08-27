
#include <cinder/app/App.h>

#include <graph/graph_ParticleSystem.h>

#include <AdjApp.h>
#include <adj/adj_Visualizer.h>
#include <adj/adj_GraphPhysics.h>
#include <adj/adj_Renderer.h>
#include <adj/adj_Camera.h>
#include <adj/adj_GraphNodeFactory.h>
#include <adj/adj_Song.h>
#include <adj/adj_PlayManager.h>
#include <adj/adj_VoteManager.h>
#include <adj/adj_User.h>
#include <adj/adj_NowPlayingHeadline.h>
#include <adj/adj_DJController.h>
//#include <adj/adj_SocialConnector.h>


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
        ci::app::console() << "Could not build song database." << std::endl;
        exit;
    }
}

void Visualizer::update() {
    GraphPhysics::instance().update();
    PlayManager::instance().update();
    VoteManager::instance().update();
    GraphNodeFactory::instance().update();
    UserFactory::instance().update();
    Camera::instance().update(); // updates centroid
    //SocialConnector::instance().update();
    NowPlayingHeadline::instance().update();
    DJController::instance().update();
}

void Visualizer::draw() {
    Renderer::instance().draw();

    NowPlayingHeadline::instance().draw();
}

void Visualizer::shutdown() {
    //SocialConnector::instance().cleanup();
    GraphNodeFactory::cleanup();
    SongFactory::cleanup();
    GraphPhysics::cleanup();
    Camera::cleanup();
    Renderer::cleanup();
}

// adding nodes should be done through votes
void Visualizer::add_node() {
    GraphNodeFactory::instance().add_new_to_random_node();
}

bool Visualizer::mouse_drag(ci::app::MouseEvent) {
    //add_node();

    return true;
}

bool Visualizer::key_down(ci::app::KeyEvent key) {

	if( key.getChar() == 'f' )
		AdjApp::instance().setFullScreen(!AdjApp::instance().isFullScreen());

	if( key.getChar() == 'q' ) {
        AdjApp::instance().shutdown();
		exit(0);
    }

    return true;
    
    if (key.getChar() == 'a')
        add_node();
    else if (key.getChar() == ' ')
        DJController::instance().transition();
    else if (key.getChar() == 's')
        PlayManager::instance().switch_to_next_song();
    //else if (key.getChar() == '1')
    //   SocialConnector::instance().test_connect();

    return true;
}

}
