
#pragma once

#include <vector>

#include "adj/adj_Adj.h"
#include "adj/adj_GraphNodeFactory.h"

namespace graph {
    class Particle;
}

namespace adj {

class Song;
typedef std::shared_ptr<Song> SongPtr;

typedef std::shared_ptr<graph::Particle> ParticlePtr;

class GraphNode;
typedef std::shared_ptr<GraphNode> GraphNodePtr;

class GraphNode {
public:
    GraphNode();

    void draw();

    // checks to see if the child has already been added 
    void add_child(GraphNodePtr);

    friend class GraphNodeFactory;

private:
    SongPtr song_;
    ParticlePtr particle_;
    GraphNodePtr parent_;
    std::vector<GraphNodePtr> children_;

    bool is_valid_; // has a song that can be played

    float size_;
};



}