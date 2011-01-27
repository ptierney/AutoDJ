
#include "cinder/app/App.h"

#include "adj/adj_CalloutConnectionDrawer.h"
#include "adj/adj_GraphNodeFactory.h"
#include "AdjApp.h"

namespace adj {

CalloutConnectionDrawer::CalloutConnectionDrawer()
    : graph_nodes_(GraphNodeFactory::instance().nodes()) {

}

void CalloutConnectionDrawer::init() {
    surface_ = std::shared_ptr<ci::cairo::SurfaceImage>(new
        ci::cairo::SurfaceImage(AdjApp::instance().getWindowWidth(),
        AdjApp::instance().getWindowHeight(), true));

    context_ = std::shared_ptr<ci::cairo::Context>(new
        ci::cairo::Context(*surface_));
}

void CalloutConnectionDrawer::draw() {

}

CalloutConnectionDrawer* CalloutConnectionDrawer::instance_ = NULL;

CalloutConnectionDrawer& CalloutConnectionDrawer::instance() {
    if (instance_ == NULL) {
        instance_ = new CalloutConnectionDrawer();
        instance_->init();
    }

    return *instance_;
}

void CalloutConnectionDrawer::cleanup() {
    if (instance_ == NULL)
        return;
    
    delete instance_;
}


}