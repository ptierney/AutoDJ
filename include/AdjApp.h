
#pragma once

#include "adj/adj_Adj.h"

#include "cinder/app/AppNative.h"

namespace adj {
    class Visualizer;
}

class AdjApp : public ci::app::AppNative {
public:
    AdjApp();

    void prepareSettings(Settings*);

    void setup();
    void update();
    void draw();

    void shutdown();

    static AdjApp& instance() { return *instance_; }

private:
    std::shared_ptr<adj::Visualizer> visualizer_;

    static AdjApp* instance_;
};

