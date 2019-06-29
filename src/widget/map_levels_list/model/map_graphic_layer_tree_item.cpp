#include "editor/graphic_layer.hpp"

#include "widget/map_levels_list/model/map_graphic_layer_tree_item.hpp"

namespace Widget {
namespace MapLevelsList {
namespace Model {
MapGraphicLayerTreeItem::MapGraphicLayerTreeItem(
    std::int8_t position,
    Editor::GraphicLayer& graphicLayer
) : m_position(position), m_graphicLayer(graphicLayer)
{
    if (m_graphicLayer.visible()) {
        setIcon(QIcon(":/icons/icon_eye.png"));
    } else {
        setIcon(QIcon(":/icons/icon_eye_crossed.png"));
    }
}

Editor::Layer& MapGraphicLayerTreeItem::layer() {
    return m_graphicLayer;
}

QVariant MapGraphicLayerTreeItem::data(int role) const {
    if (role == Qt::DisplayRole) {
        return QStringLiteral("Layer");
    }
    return QStandardItem::data(role);
}
} // namespace Model
} // namespace MapLevelsList
} // namespace Widget
