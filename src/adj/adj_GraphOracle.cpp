
#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"

#include "adj/adj_GraphOracle.h"
#include "adj/adj_GraphNodeFactory.h"
#include "adj/adj_GraphNode.h"

namespace adj {


SingleGraphOracleQuery::SingleGraphOracleQuery(SongId id) 
    : id_(id) {
    // nothing here
}

// THIS IS RUNS IN A SEPARATE THREAD
void SingleGraphOracleQuery::operator()() {

}

MultipleGraphOracleQuery::MultipleGraphOracleQuery(
    std::shared_ptr<std::vector<SongId> > ids) : ids_(*ids) {
    // nothing here
}

// THIS IS RUNS IN A SEPARATE THREAD
void MultipleGraphOracleQuery::operator()() {

}


GraphOracle::GraphOracle()
    : graph_nodes_(GraphNodeFactory::instance().nodes()), 
    song_map_(GraphNodeFactory::instance().song_map()) {
    
    max_threads_ = 5;
}

void GraphOracle::init() {
    // nothing here
}

void GraphOracle::query_best_connection(SongId id) {
    // THIS IS DUMMY CODE TO BE REPLACED

    SongId other_id = SongFactory::instance().get_random_song_id();

    while (other_id == id || (GraphNodeFactory::instance().song_map().find(
        other_id) == GraphNodeFactory::instance().song_map().end())) {
        other_id = SongFactory::instance().get_random_song_id();
    }

    GraphNodeFactory::instance().add_pair_request(PairRequest(id, other_id));

    return;
    
    // BEGIN ACTUAL CODE!!!

   SingleGraphOracleQuery query(id);

   prune_threads();

   threads_.push_back(ThreadPtr(new boost::thread(query)));
}

void GraphOracle::query_best_connection(std::shared_ptr<std::vector<SongId> > ids) {
    MultipleGraphOracleQuery query(ids);

    prune_threads();

    threads_.push_back(ThreadPtr(new boost::thread(query)));
}

void GraphOracle::prune_threads() {
    if (threads_.size() < max_threads_)
        return;

    threads_.front()->join();
    threads_.pop_front();
}

GraphOracle* GraphOracle::instance_ = NULL;

GraphOracle& GraphOracle::instance() {
    if (instance_ == NULL) {
        instance_ = new GraphOracle();
        instance_->init();
    }

    return *instance_;
}

}
