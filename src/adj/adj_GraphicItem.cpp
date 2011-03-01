
#include <adj/adj_GraphicItem.h>
#include <adj/adj_Renderer.h>

namespace adj {

GraphicItem::GraphicItem() {
    Renderer::instance().register_new_graphic_item(this);
}

}