#include "widget/map_floors_list/widget.hpp"
#include "widget/map_floors_list/ui_widget.h"

#include <QDebug>
#include "widget/map_floors_list/model/map_floor_tree_item.hpp"
#include "widget/map_floors_list/model/map_floor_tree_model.hpp"

namespace MapFloorsList {

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::MapFloorsList)
    , m_editorMap(nullptr)
    , m_mapFloorTreeModel(nullptr)
    , m_mapScene(nullptr)
{
    m_ui->setupUi(this);
}

void Widget::setEditorMap(std::shared_ptr<Editor::Map> editorMap) {
    m_editorMap = editorMap;
    reset();
    m_mapFloorTreeModel = new MapFloorTreeModel(m_editorMap);
    m_ui->treeViewFloors->setModel(m_mapFloorTreeModel);
}

void Widget::reset() {
    if (m_mapFloorTreeModel != nullptr) {
        delete m_mapFloorTreeModel;
        m_mapFloorTreeModel = nullptr;
    }
    m_ui->treeViewFloors->reset();
}

void Widget::toggleLayerVisibleState(QModelIndex selectedIndex) {
    qDebug() << "selectedIndex: " << selectedIndex;
   m_mapFloorTreeModel->mapTreeItemFromIndex(selectedIndex)->toggle();
}

void Widget::selectLayer(QModelIndex selectedIndex) {
    auto item = m_mapFloorTreeModel->mapTreeItemFromIndex(selectedIndex);
    // This line will trigger a signal:
    item->setSelected();
}

} // namespace MapFloorsList
