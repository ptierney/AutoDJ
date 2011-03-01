
#include <cinder/gl/gl.h>
#include <cinder/CinderMath.h>

#include <graph/graph_ParticleSystem.h>
#include <graph/graph_Particle.h>

#include <adj/adj_Camera.h>
#include <AdjApp.h>
#include <adj/adj_GraphPhysics.h>
#include <adj/adj_CalloutBox.h>
#include <adj/adj_Renderer.h>

namespace adj {

Camera* Camera::instance_ = NULL;

Camera::Camera() {
    // the circles are approximately 10 units wide
    width_border_ = 15.0f; 
    height_border_ = 15.0f;
    scale_ = 1.0f;
    centroid_ = ci::Vec2f::zero();

    scale_damping_ = 0.01;
    centroid_damping_ = 0.01;
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
    std::vector<GraphicItem*>& g_items = 
        Renderer::instance().graphic_items();

    if (g_items.size() < 2)
        return;

    GraphicItem& first = *(g_items[0]);

    float x_min = first.get_pos_x();
    float x_max = first.get_pos_y();
    float y_min = first.get_pos_y();
    float y_max = first.get_pos_y();

    for (std::vector<GraphicItem*>::iterator it = g_items.begin();
        it != g_items.end(); ++it) {

        GraphicItem& g = *(*it);
        float half_w = g.get_width() / 2.0f;
        float half_h = g.get_height() / 2.0f;

        x_max = ci::math<float>::max(x_max, g.get_pos_x() + half_w);
        x_min = ci::math<float>::min(x_min, g.get_pos_x() - half_w);
        y_min = ci::math<float>::min(y_min, g.get_pos_y() - half_h);
        y_max = ci::math<float>::max(y_max, g.get_pos_y() + half_h);
    }

    float delta_x = x_max - x_min;
    float delta_y = y_max - y_min;

    float target_x = x_min + 0.5f * delta_x;
    float target_y = y_min + 0.5f * delta_y;

    centroid_.x += (target_x - centroid_.x) * centroid_damping_;
    centroid_.y += (target_y - centroid_.y) * centroid_damping_;

    centroid_ *= -1.0f;

    float width_scale  = static_cast<float>(AdjApp::instance().getWindowWidth()) / 
        (delta_x + width_border_);
    float height_scale = static_cast<float>(AdjApp::instance().getWindowHeight()) / 
        (delta_y + height_border_);

    scale_ += (ci::math<float>::min(width_scale, height_scale) - scale_) * scale_damping_;
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
