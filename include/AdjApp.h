
#pragma once

#if defined (CINDER_MAC)
    #include <tr1/memory>
#endif

#include "cinder/app/AppBasic.h"

namespace adj {
    class Visualizer;
}

class AdjApp : public ci::app::AppBasic {
public:
    AdjApp();

    void prepareSettings(Settings*);

    void setup();
    void update();
    void draw();

    static AdjApp& instance() { return *instance_; }

private:
    std::tr1::shared_ptr<adj::Visualizer> visualizer_;

    static AdjApp* instance_;
};

