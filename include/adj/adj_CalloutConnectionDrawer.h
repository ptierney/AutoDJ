
#pragma once

#include "adj/adj_Adj.h"

#include <vector>

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Color.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Text.h"
#include "cinder/Font.h"

namespace adj {

typedef std::shared_ptr<class GraphNode> GraphNodePtr;

class CalloutConnectionDrawer { 
public:
    static CalloutConnectionDrawer& instance();
    static void cleanup();

    void draw();

private:
    CalloutConnectionDrawer();
    void init();

    std::shared_ptr<ci::cairo::SurfaceImage> surface_;
    std::shared_ptr<ci::cairo::Context> context_;

    static CalloutConnectionDrawer* instance_;

    std::vector<GraphNodePtr>& graph_nodes_;


};

}