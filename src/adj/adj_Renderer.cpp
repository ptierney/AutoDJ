
#include <deque>
#include <algorithm>

#include <cinder/gl/gl.h>
#include <cinder/Color.h>

#include <graph/graph_Particle.h>
#include <graph/graph_ParticleSystem.h>
#include <graph/graph_Spring.h>

#include <AdjApp.h>
#include <adj/adj_Renderer.h>
#include <adj/adj_Camera.h>
#include <adj/adj_GraphPhysics.h>
#include <adj/adj_GraphNodeFactory.h>
#include <adj/adj_GraphNode.h>
#include <adj/adj_GraphicItem.h>

namespace adj {

Renderer::Renderer() {
    node_size_ = 10.0f;

    line_width_ = 6.0f;
    highlight_width_ = line_width_ * 2.0f;
    
    // Pantone 307 
    color_palette_.push_back(ci::ColorA(0.f / 255.f, 115.f / 255.f, 
        164.f / 255.f, 1.f));

    // Pantone 305 
    color_palette_.push_back(ci::ColorA(92.f / 255.f, 205.f / 255.f, 
        235.f / 255.f, 1.f));

    // Pantone 368 
    color_palette_.push_back(ci::ColorA(97.f / 255.f, 181.f / 255.f, 
        68.f / 255.f, 1.f));

    // Pantone 7408 
    color_palette_.push_back(ci::ColorA(242.f / 255.f, 175.f / 255.f, 0.f, 1.f));
    
    // Pantone 1797 
    color_palette_.push_back(ci::ColorA(196.f / 255.f, 38.f / 255.f, 
        46.f / 255.f, 1.f));

    background_color_ = ci::Color::black();
    network_color_ = color_palette_[1];
}

void Renderer::init() {
    p_system_ = GraphPhysics::instance().particle_system();
}

void Renderer::setup() {
    // line widths aren't working atm
    //glEnable(GL_LINE_SMOOTH);
    ci::gl::enableAlphaBlending();
}

void Renderer::draw() {
    ci::gl::setMatricesWindow(AdjApp::instance().getWindowSize());
    ci::gl::clear(background_color_);

    ci::gl::pushMatrices();

        Camera::instance().transform_draw();

        draw_connections();
        draw_nodes();

    ci::gl::popMatrices();
}

void Renderer::draw_nodes() {
    for (std::vector<GraphNodePtr>::iterator it = GraphNodeFactory::instance().nodes().begin();
        it != GraphNodeFactory::instance().nodes().end(); ++it) {

        (*it)->draw();
        (*it)->draw_node_body();
    }

    for (std::vector<GraphNodePtr>::iterator it = GraphNodeFactory::instance().nodes().begin();
        it != GraphNodeFactory::instance().nodes().end(); ++it) {

        (*it)->draw_callout_box();
    }
}

// in the future, the nodes will probably draw their own connections, or at least
// store their line weight based on their distance to "now playing"
void Renderer::draw_connections() {
    glBegin(GL_LINES);

    std::vector<std::pair<GraphNodePtr, GraphNodePtr> >& edges = 
        GraphNodeFactory::instance().edges();

    for (std::vector<std::pair<GraphNodePtr, GraphNodePtr> >::iterator it = 
        edges.begin(); it != edges.end(); ++it) {

        // removed due to gl line weight problems
        //if (it->first->highlight_connection()) {
        //    glLineWidth(highlight_width_);
        //    ci::gl::color(it->first->node_highlight_color());
        //}

        glLineWidth(line_width_);
        ci::gl::color(network_color_);

        glVertex2f(it->first->particle()->position());
        glVertex2f(it->second->particle()->position());
    }

    glEnd();
}

void Renderer::register_new_graphic_item(GraphicItem* item) {
    if (std::find(graphic_items_.begin(), graphic_items_.end(), item) !=
        graphic_items_.end())
        return;

    graphic_items_.push_back(item);
}

void Renderer::remove_graphic_item(GraphicItem* item) {
    std::vector<GraphicItem*>::iterator it = std::find(graphic_items_.begin(), 
        graphic_items_.end(), item);

    if (it == graphic_items_.end())
        return;

    graphic_items_.erase(it);
}

Renderer* Renderer::instance_ = NULL;

Renderer& Renderer::instance() {
    if (instance_ == NULL) {
        instance_ = new Renderer();
        instance_->init();
    }

    return *instance_;
}

void Renderer::cleanup() {
    if (instance_ == NULL)
        return;

    delete instance_;
}

}