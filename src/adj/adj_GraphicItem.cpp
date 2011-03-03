
#include <adj/adj_GraphicItem.h>
#include <adj/adj_Renderer.h>

namespace adj {

GraphicItem::GraphicItem() {
    Renderer::instance().register_new_graphic_item(this);
}

GraphicItem::~GraphicItem() {
    Renderer::instance().remove_graphic_item(this);
}

}