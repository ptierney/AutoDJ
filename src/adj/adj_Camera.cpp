
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"

#include "graph/graph_ParticleSystem.h"
#include "graph/graph_Particle.h"

#include "adj/adj_Camera.h"
#include "AdjApp.h"
#include "adj/adj_GraphPhysics.h"

namespace adj {

Camera* Camera::instance_ = NULL;

Camera::Camera() {
    width_border_ = 50.0f;
    height_border_ = 50.0f;
    scale_ = 1.0f;
    centroid_ = ci::Vec2i::zero();
}

void Camera::init() {
    // nothing here
}

void Camera::setup() {
    p_system_ = GraphPhysics::instance().particle_system();
}

void Camera::update() {
    // monitor time and update tweening
    update_centroid();
}

void Camera::transform_draw() {
    ci::gl::translate(static_cast<ci::Vec2f>(
        AdjApp::instance().getWindowSize()) / 2.0f);
    ci::gl::scale(ci::Vec3f::one() * scale_);
    ci::gl::translate(centroid_);
}

void Camera::update_centroid() {
    if (p_system_->particles_.size() < 2)
        return;

    ParticlePtr first = p_system_->particles_[0];

    float x_min = first->position().x;
    float x_max = first->position().x;
    float y_min = first->position().y;
    float y_max = first->position().y;

    for (std::vector<ParticlePtr>::iterator it = p_system_->particles_.begin();
        it != p_system_->particles_.end(); ++it) {

        graph::Particle& p = *((*it).get());

        x_max = ci::math<float>::max(x_max, p.position().x);
        x_min = ci::math<float>::min(x_min, p.position().x);
        y_min = ci::math<float>::min(y_min, p.position().y);
        y_max = ci::math<float>::max(y_max, p.position().y);
    }

    float delta_x = x_max - x_min;
    float delta_y = y_max - y_min;

    centroid_.x = x_min + 0.5f * delta_x;
    centroid_.y = y_min + 0.5f * delta_y;

    if (delta_y > delta_x)
        scale_ = AdjApp::instance().getWindowHeight() / (delta_y + 50.0f);
    else
        scale_ = AdjApp::instance().getWindowWidth() / (delta_x + 50.0f);
}

Camera& Camera::instance() {
    if (instance_ == NULL) {
        instance_ = new Camera();
        instance_->init();
    }

    return *instance_;
}

void Camera::cleanup() {
    if (instance_ == NULL)
        return;

    delete instance_;
}




}
