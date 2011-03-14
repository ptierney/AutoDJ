
#include <cinder/gl/gl.h>
#include <cinder/CinderMath.h>

#include <graph/graph_ParticleSystem.h>
#include <graph/graph_Particle.h>

#include <adj/adj_Camera.h>
#include <AdjApp.h>
#include <adj/adj_GraphPhysics.h>
#include <adj/adj_CalloutBox.h>
#include <adj/adj_Renderer.h>
#include <adj/adj_NowPlayingHeadline.h>
#include <adj/adj_GraphNodeFactory.h>
#include <adj/adj_GraphNode.h>

namespace adj {

Camera* Camera::instance_ = NULL;

Camera::Camera() {
    // the circles are approximately 10 units wide
    width_border_ = 40.0f; 
    height_border_ = 40.0f;
    scale_ = 1.0f;
    centroid_ = ci::Vec2f::zero();

    scale_damping_ = 0.03;
    centroid_damping_ = 0.03;
    
    node_crop_cuttoff_ = 5;
}

void Camera::init() {
    // nothing here
}

void Camera::setup() {
    p_system_ = GraphPhysics::instance().particle_system();
    box_p_system_ = GraphPhysics::instance().box_particle_system();
}

void Camera::update() {
    update_centroid();
}

void Camera::transform_draw() {
    ci::gl::scale(ci::Vec3f::one() * scale_);
    ci::gl::translate(centroid_);
}

void Camera::get_min_max_over_all(float& x_min, float& x_max,
    float& y_min, float& y_max) {
    
    std::vector<GraphicItem*>& g_items = 
        Renderer::instance().graphic_items();
    
    GraphicItem& first = *(g_items[0]);

    x_min = first.get_pos_x();
    x_max = first.get_pos_x();
    y_min = first.get_pos_y();
    y_max = first.get_pos_y();

    for (std::vector<GraphicItem*>::iterator it = g_items.begin();
        it != g_items.end(); ++it) {

        GraphicItem& g = *(*it);

        if (!g.visible())
            continue;

        float half_w = g.get_width() / 2.0f;
        float half_h = g.get_height() / 2.0f;

        x_max = ci::math<float>::max(x_max, g.get_pos_x() + half_w);
        x_min = ci::math<float>::min(x_min, g.get_pos_x() - half_w);
        y_min = ci::math<float>::min(y_min, g.get_pos_y() - half_h);
        y_max = ci::math<float>::max(y_max, g.get_pos_y() + half_h);
    }
}
        
void Camera::get_min_max_over_active(float& x_min, float& x_max,
    float& y_min, float& y_max) {
    
    std::vector<GraphicItem*> g_items;
    
    // look for active nodes and their callout boxes
    
    std::vector<GraphNodePtr>& nodes = GraphNodeFactory::instance().nodes();
    
    for (std::vector<GraphNodePtr>::iterator it = nodes.begin();
        it != nodes.end(); ++it) {
        
        if (!((*it)->is_current_song() || (*it)->callout_box().visible()))
            continue;
            
        g_items.push_back(it->get());
        g_items.push_back(&((*it)->callout_box()));
    }
    
    if (g_items.size() < 3) {
        get_min_max_over_all(x_min, x_max, y_min, y_max);
        return;
    }
    
    GraphicItem& first = *(g_items[0]);

    x_min = first.get_pos_x();
    x_max = first.get_pos_x();
    y_min = first.get_pos_y();
    y_max = first.get_pos_y();

    for (std::vector<GraphicItem*>::iterator it = g_items.begin();
        it != g_items.end(); ++it) {

        GraphicItem& g = *(*it);

        if (!g.visible())
            continue;

        float half_w = g.get_width() / 2.0f;
        float half_h = g.get_height() / 2.0f;

        x_max = ci::math<float>::max(x_max, g.get_pos_x() + half_w);
        x_min = ci::math<float>::min(x_min, g.get_pos_x() - half_w);
        y_min = ci::math<float>::min(y_min, g.get_pos_y() - half_h);
        y_max = ci::math<float>::max(y_max, g.get_pos_y() + half_h);
    }
}


void Camera::update_centroid() {
    std::vector<GraphicItem*>& g_items = 
        Renderer::instance().graphic_items();

    float x_min;
    float x_max;
    float y_min;
    float y_max;

    if (g_items.size() < 2)
        return;
    else if (g_items.size() < node_crop_cuttoff_)
        get_min_max_over_all(x_min, x_max, y_min, y_max);
    else
        get_min_max_over_active(x_min, x_max, y_min, y_max);

    float delta_x = x_max - x_min;
    float delta_y = y_max - y_min;

    float target_x = x_min;
    float target_y = y_min;

    target_x -= width_border_ / scale_;
    target_y -= height_border_ / scale_;
    
    target_y -= NowPlayingHeadline::instance().size().y / scale_;

    ci::Vec2f centroid_target = ci::Vec2f(target_x, target_y);

    centroid_target *= -1.0f;

    float width_scale  = static_cast<float>(AdjApp::instance().getWindowWidth() - 
        2 * width_border_) / 
        (delta_x);
    float height_scale = static_cast<float>(AdjApp::instance().getWindowHeight() - 
        2 * height_border_ - NowPlayingHeadline::instance().size().y) / 
        (delta_y);

    float abs_scale = ci::math<float>::min(width_scale, height_scale);

    if (width_scale < height_scale)
        centroid_target.y += (AdjApp::instance().getWindowHeight() / scale_ - delta_y) / 2.0f;
    else
        centroid_target.x += (AdjApp::instance().getWindowWidth() / scale_ - delta_x) / 2.0f;

    centroid_ += (centroid_target - centroid_) * centroid_damping_;

    scale_ += (abs_scale - scale_) * scale_damping_;
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
