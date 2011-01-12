
#include "cinder/gl/gl.h"

#include "AdjApp.h"
#include "adj/adj_Visualizer.h"

namespace adj {

Visualizer::Visualizer() {
    // nothing here
}

void Visualizer::setup() {
    glEnable(GL_LINE_SMOOTH);
    ci::gl::enableAlphaBlending();
}

void Visualizer::update() {
    // nothing here
}

void Visualizer::draw() {
    ci::gl::setMatricesWindow(AdjApp::instance().getWindowSize());
    ci::gl::clear(ci::Color::white());

    ci::gl::color(ci::Color::black());

    glBegin(GL_LINES);
    glVertex2f(0.0, 0.0);
    glVertex2f(AdjApp::instance().getMousePos().x,
        AdjApp::instance().getMousePos().y);
    glEnd();
}

}
