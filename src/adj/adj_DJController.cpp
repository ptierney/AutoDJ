
#include <cassert>
#include <utility>

#include <graph/graph_ParticleSystem.h>

#include <adj/adj_DJController.h>
#include <adj/adj_GraphPhysics.h>
#include <adj/adj_PlayManager.h>
#include <adj/adj_GraphNodeFactory.h>
#include <adj/adj_GraphNode.h>

namespace adj {

DJController* DJController::instance_ = NULL;

DJController::DJController() {
    has_next_song_ = false;
}

void DJController::init() {
    // nothing here
}

void DJController::transition() {
    if (!has_next_song_)
        set_next_song_randomly();

    // delete all attractions
    GraphPhysics::instance().clear_all_node_connections();

    GraphNodePtr old_song = PlayManager::instance().now_playing();

    // update now playing to new node
    PlayManager::instance().set_now_playing(
        GraphNodeFactory::instance().song_map()[next_song_]);

    // delete now playing node
    GraphNodeFactory::instance().delete_node(old_song);

    // make sure this method works
    assert(old_song.unique()); 

    // this call should actually delete the object
    old_song.reset();



    // make "choose 2" separations between nodes
    ParticleSystemPtr system = GraphPhysics::instance().particle_system();
    std::vector<ParticlePtr>& particles = system->particles_;

    for (std::vector<ParticlePtr>::iterator it_a = particles.begin();
        it_a != particles.end(); ++it_a) {

        for (std::vector<ParticlePtr>::iterator it_b = particles.begin();
            it_b != particles.end(); ++it_b) {

            if (*it_a == *it_b)
                continue;

            GraphPhysics::instance().make_separation_between_node_particles(*it_a, *it_b);
        }
    }

    // make n-1 springs to now playing

    GraphNodePtr now_playing = PlayManager::instance().now_playing();
    now_playing->set_is_current_song(true);

    std::vector<GraphNodePtr>& nodes = GraphNodeFactory::instance().nodes();

    for (std::vector<GraphNodePtr>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        if (*it == now_playing)
            continue;

        GraphPhysics::instance().link_nodes(now_playing, *it);
        GraphNodeFactory::instance().add_edge(std::pair<GraphNodePtr, GraphNodePtr>(
            now_playing, *it));
    }

    // add edges

    has_next_song_ = false;
}

void DJController::set_next_song(SongId id) {
    next_song_ = id;
    has_next_song_ = true;
}

void DJController::set_next_song_randomly() {
    GraphNodePtr node = PlayManager::instance().get_next_song_randomly();

    if (node.get() == NULL)
        assert(0); // epic fail

    next_song_ = node->song().id();
    has_next_song_ = true;
}

DJController& DJController::instance() {
    if (instance_ == NULL) {
        instance_ = new DJController();
        instance_->init();
    }

    return *instance_;
}




}