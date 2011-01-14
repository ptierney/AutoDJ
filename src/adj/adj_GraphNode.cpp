
#include "adj/adj_GraphNode.h"

namespace adj {

GraphNode::GraphNode() {
    is_valid_ = false;
}

GraphNode::GraphNode(SongPtr s) : song_(s) {

}


}