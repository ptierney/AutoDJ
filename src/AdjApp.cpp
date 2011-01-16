
#include "cinder/app/AppBasic.h"

#include "AdjApp.h"
#include "adj/adj_Visualizer.h"

AdjApp* AdjApp::instance_;

AdjApp::AdjApp() {
    instance_ = this;
}

void AdjApp::prepareSettings(Settings* settings) {
	settings->setWindowSize(1000, 700);
	settings->setFrameRate(60.0f);
	settings->setResizable(false);
	settings->setFullScreen(false);
    settings->setTitle("Auto DJ");
}

void AdjApp::setup() {
    visualizer_ = std::shared_ptr<adj::Visualizer>(
        new adj::Visualizer());

    visualizer_->setup();
}

void AdjApp::update() {
    visualizer_->update();
}

void AdjApp::draw() {
    visualizer_->draw();
}

void AdjApp::shutdown() {
    visualizer_->shutdown();

    visualizer_.reset();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC(AdjApp, ci::app::RendererGl)

