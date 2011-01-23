
#pragma once

#include <deque>
#include <utility>

// this class allows you to look up similarities between songs

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"

#include "adj/adj_Song.h"

namespace adj {

typedef std::shared_ptr<class GraphNode> GraphNodePtr;
// pair<a, b> ==> a connects to b
typedef std::vector<std::pair<SongId, SongId> > EdgeList;
typedef std::shared_ptr<EdgeList> EdgeListPtr;

typedef std::shared_ptr<boost::thread> ThreadPtr;

typedef std::pair<SongId, SongId> PairRequest;

class GraphOracleQuery {
public:
    virtual void operator()() = 0;
};

class SingleGraphOracleQuery : public GraphOracleQuery {
public:
    SingleGraphOracleQuery(SongId);
    
    virtual void operator()();

private:
    SongId id_;
};

class MultipleGraphOracleQuery : public GraphOracleQuery {
public:
    MultipleGraphOracleQuery(std::shared_ptr<std::vector<SongId> >);

    virtual void operator()();

private:
    std::vector<SongId> ids_;
};

class GraphOracle {
public:
    static GraphOracle& instance();
    static void cleanup();

    // given a song, it returns the best song to attatch 

    // used for a new song
    // add SongId to GraphNodeFactory
    void query_best_connection(SongId);
    // used when a song end and the graph restructures
    // adds EdgeListPtr to GraphNodeFactory
    void query_best_connection(std::shared_ptr<std::vector<SongId> >);

private:
    GraphOracle();
    void init();
    void prune_threads(); // deletes oldest thread

    std::deque<ThreadPtr> threads_;

    const std::vector<GraphNodePtr>& graph_nodes_;
    const std::map<SongId, GraphNodePtr>& song_map_;

    int max_threads_;

    static GraphOracle* instance_;
};

}