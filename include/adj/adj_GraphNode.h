
#pragma once

namespace adj {

class Song;

class GraphNode {
public:
    GraphNode(Song&);

private:
    Song& song_;
};

}