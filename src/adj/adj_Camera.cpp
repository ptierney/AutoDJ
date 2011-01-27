
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"

#include "graph/graph_ParticleSystem.h"
#include "graph/graph_Particle.h"

#include "adj/adj_Camera.h"
#include "AdjApp.h"
#include "adj/adj_GraphPhysics.h"
#include "adj/adj_CalloutBox.h"

namespace adj {

Camera* Camera::instance_ = NULL;

Camera::Camera() {
    // the circles are approximately 10 units wide
    width_border_ = 5.0f; 
    height_border_ = 5.0f;
    scale_ = 1.0f;
    centroid_ = ci::Vec2f::zero();
}

void Camera::init() {
    // nothing here
}

void Camera::setup() {
    p_system_ = GraphPhysics::instance().particle_system();
    box_p_system_ = GraphPhysics::instance().box_particle_system();
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

    // check all node particles for bounds
    for (std::vector<ParticlePtr>::iterator it = p_system_->particles_.begin();
        it != p_system_->particles_.end(); ++it) {

        graph::Particle& p = *(*it);

        x_max = ci::math<float>::max(x_max, p.position().x);
        x_min = ci::math<float>::min(x_min, p.position().x);
        y_min = ci::math<float>::min(y_min, p.position().y);
        y_max = ci::math<float>::max(y_max, p.position().y);
    }

    float width_max = CalloutBox::max_width();
    float height_max = CalloutBox::max_height();

    ci::app::console() << width_max << " " << height_max << std::endl;

    // check all the box particles for their bounds
    for (std::vector<ParticlePtr>::iterator it = box_p_system_->particles_.begin();
        it != box_p_system_->particles_.end(); ++it) {

        graph::Particle& p = *(*it);

        x_max = ci::math<float>::max(x_max, p.position().x + width_max);
        x_min = ci::math<float>::min(x_min, p.position().x - width_max);
        y_min = ci::math<float>::min(y_min, p.position().y - height_max);
        y_max = ci::math<float>::max(y_max, p.position().y + height_max);
    }

    float delta_x = x_max - x_min;
    float delta_y = y_max - y_min;

    centroid_.x = x_min + 0.5f * delta_x;
    centroid_.y = y_min + 0.5f * delta_y;

    centroid_ *= -1.0f;

    float width_scale  = static_cast<float>(AdjApp::instance().getWindowWidth()) / 
        (delta_x + width_border_);
    float height_scale = static_cast<float>(AdjApp::instance().getWindowHeight()) / 
        (delta_y + height_border_);

    scale_ = ci::math<float>::min(width_scale, height_scale);
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
