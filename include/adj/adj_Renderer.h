
#pragma once

#include <vector>

#include <cinder/Color.h>

namespace graph {
    class Particle;
    class ParticleSystem;
}

namespace adj {

class GraphicItem;

typedef std::shared_ptr<graph::Particle> ParticlePtr;
typedef std::shared_ptr<graph::ParticleSystem> ParticleSystemPtr;

class Renderer {
public:
    static Renderer& instance();
    static void cleanup();

    void setup();
    void draw();

    const ci::Color& background_color() { return background_color_; } 

    void register_new_graphic_item(GraphicItem*);
    void remove_graphic_item(GraphicItem*);

    std::vector<GraphicItem*>& graphic_items() { return graphic_items_; }

private:
    Renderer();
    void init();

    void draw_nodes();
    void draw_connections();

    static Renderer* instance_;

    ci::Color background_color_;
    ci::ColorA network_color_;

    float line_width_;
    float highlight_width_;
    float node_size_;

    ParticleSystemPtr p_system_;

    std::vector<GraphicItem*> graphic_items_;

};

}