#include "drawing_tool/graphic/graphic_tool.hpp"

#include "graphicmap/mapgraphicsscene.hpp"

#include "widget/drawing_toolbar/widget.hpp"
#include "widget/drawing_toolbar/state/display_blocking_tools.hpp"
#include "widget/drawing_toolbar/state/display_graphic_tools.hpp"

namespace DrawingToolbar {

DisplayGraphicToolState::DisplayGraphicToolState(Widget& widget)
    : State(widget)
{}

void
DisplayGraphicToolState::visitGraphicLayer(GraphicMap::BlockingGraphicLayer& layer)
{
    // Here we will reset tools and change state.
    auto self(shared_from_this());
    m_widget.reset();
    m_widget.setState(std::make_shared<DisplayBlockingToolState>(m_widget));
}

void
DisplayGraphicToolState::visitGraphicLayer(GraphicMap::VisibleGraphicLayer& layer)
{
    // Nothing to do here. We are already displaying accurate tools.
    auto tool = m_widget.mapScene()->drawingTool();

    if (nullptr != tool) {
        // XXX: Disgusting.
        reinterpret_cast<DrawingTools::GraphicTool*>(
            tool
        )->setVisibleGraphicLayer(&layer);
    }
}

} // namespace DrawingToolbar
