
#pragma once

#include "adj/adj_Adj.h"

namespace adj {

class Song;
typedef std::tr1::shared_ptr<Song> SongPtr;

class GraphNode {
public:
    GraphNode();
    GraphNode(SongPtr);

private:
    SongPtr song_;
    bool is_valid_; // has a song that can be played
};



}