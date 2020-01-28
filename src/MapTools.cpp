#include "MapTools.hpp"
#include "ui_GeneralWindow.h"

#include "editor/layerBlocking.hpp"
#include "editor/layerGraphic.hpp"
#include "graphicMap/layerGraphicBlocking.hpp"
#include "graphicMap/layerGraphicVisible.hpp"
#include "graphicMap/mapGraphicsScene.hpp"
#include "utils/definitions.hpp"

MapTools::MapTools(ChipsetGraphicsScene& chipset, GraphicMap::MapGraphicsScene& map, Ui::GeneralWindow& ui)
    : m_chipsetScene(chipset)
    , m_mapScene(map)
    , m_toolsUI(ui)
{
    m_mapScene.linkToolSet(this);
}

void MapTools::clear()
{
    resetTools();
    resetLayerLink();
}

void MapTools::setActiveLayer(GraphicMap::VisibleGraphicLayer* layer)
{
    resetLayerLink();
    m_mapScene.clearSelectRect();

    // update grid
    m_uiLayerW   = layer->layer().width();
    m_uiLayerH   = layer->layer().height();
    m_uiGridStep = CELL_H;
    m_toolsUI.actionToggleGrid->setChecked(true);

    m_currLayerType = eLayerType::Visible;
    m_visLayer      = layer;

    updateGridDisplay();
}

void MapTools::setActiveLayer(GraphicMap::BlockingGraphicLayer* layer)
{
    resetLayerLink();
    m_mapScene.clearSelectRect();

    // update grid
    m_uiLayerW   = layer->layer().width();
    m_uiLayerH   = layer->layer().height();
    m_uiGridStep = BLOCK_H;
    m_toolsUI.actionToggleGrid->setChecked(true);

    m_currLayerType = eLayerType::Blocking;
    m_blockLayer    = layer;

    updateGridDisplay();
}

void MapTools::setActiveLayer(GraphicMap::EventGraphicLayer* layer)
{
    clear();

    m_currLayerType = eLayerType::Event;
    m_eventLayer    = layer;

    updateGridDisplay();
}

void MapTools::updateGridDisplay()
{
    if (m_toolsUI.actionToggleGrid->isChecked() && m_currLayerType != eLayerType::None)
        m_mapScene.drawGrid(m_uiLayerW, m_uiLayerH, m_uiGridStep);
    else
        m_mapScene.clearGrid();
}

void MapTools::resetTools()
{
    m_toolsUI.actionPen->setChecked(false);
    m_toolsUI.actionEraser->setChecked(false);
    m_toolsUI.actionSelection->setChecked(false);
    m_toolsUI.actionPaste->setChecked(false);

    m_toolsUI.actionCopy->setEnabled(false);
    m_toolsUI.actionCut->setEnabled(false);
}

void MapTools::resetLayerLink()
{
    m_currLayerType = eLayerType::None;
    m_visLayer      = nullptr;
    m_blockLayer    = nullptr;
    m_eventLayer    = nullptr;
}

void MapTools::setPen()
{
    resetTools();
    m_toolsUI.actionPen->setChecked(true);
    m_currMode = eTools::Pen;
}

void MapTools::setEraser()
{
    resetTools();
    m_toolsUI.actionEraser->setChecked(true);
    m_currMode = eTools::Eraser;
}

void MapTools::setSelectTool()
{
    resetTools();
    m_toolsUI.actionSelection->setChecked(true);
    m_currMode = eTools::Selection;

    m_toolsUI.actionCopy->setEnabled(true);
    m_toolsUI.actionCut->setEnabled(true);
}

void MapTools::setPasteTool()
{
    resetTools();
    m_toolsUI.actionPaste->setChecked(true);
    m_currMode = eTools::Paste;
}

QPoint MapTools::adjustOnGrid(const QPoint& pxCoords)
{
    int iStep = static_cast<int>(m_uiGridStep);

    QPoint cellsCoords;
    cellsCoords.setX(pxCoords.x() - (pxCoords.x() % iStep));
    cellsCoords.setY(pxCoords.y() - (pxCoords.y() % iStep));

    return cellsCoords;
}

QRect MapTools::adjustOnGrid(const QRect& rawRect)
{
    int iStep = static_cast<int>(m_uiGridStep);

    QRect normalized = rawRect.normalized();
    QPoint p1        = adjustOnGrid(normalized.topLeft());
    QPoint p2        = adjustOnGrid(normalized.bottomRight());
    p2 += QPoint(iStep - 1, iStep - 1);

    forceInScene(p1);
    forceInScene(p2);

    return QRect(p1, p2);
}

void MapTools::forceInScene(QPoint& point)
{
    int maxX = static_cast<int>(m_uiLayerW * m_uiGridStep) - 1;
    int maxY = static_cast<int>(m_uiLayerH * m_uiGridStep) - 1;

    if (point.x() < 0)
        point.setX(0);
    if (point.x() > maxX)
        point.setX(maxX);

    if (point.y() < 0)
        point.setY(0);
    if (point.y() > maxY)
        point.setY(maxY);
}

void MapTools::eraseVisible(const QRect& region)
{
    if (m_currLayerType != eLayerType::Visible || m_visLayer == nullptr)
        return;

    quint16 minX = static_cast<quint16>(region.left() / CELL_W);
    quint16 maxX = static_cast<quint16>(region.right() / CELL_W);
    quint16 minY = static_cast<quint16>(region.top() / CELL_H);
    quint16 maxY = static_cast<quint16>(region.bottom() / CELL_H);

    for (quint16 x = minX; x <= maxX; ++x)
        for (quint16 y = minY; y <= maxY; ++y)
            m_visLayer->setTile(x, y, -1, -1);

    m_mapScene.update(region);
}

void MapTools::eraseBlocking(const QRect& region)
{
    if (m_currLayerType != eLayerType::Blocking || m_blockLayer == nullptr)
        return;

    quint16 minX = static_cast<quint16>(region.left() / BLOCK_W);
    quint16 maxX = static_cast<quint16>(region.right() / BLOCK_W);
    quint16 minY = static_cast<quint16>(region.top() / BLOCK_H);
    quint16 maxY = static_cast<quint16>(region.bottom() / BLOCK_H);

    for (quint16 x = minX; x <= maxX; ++x)
        for (quint16 y = minY; y <= maxY; ++y)
            m_blockLayer->setTile(x, y, false);

    m_mapScene.update(region);
}

void MapTools::previewTool(const QRect& clickingRegion)
{
    QRect adjustedRegion = adjustOnGrid(clickingRegion);

    switch (m_currMode) {
    case eTools::Pen:
        break;

    case eTools::Eraser:
        m_mapScene.setSelectRect(adjustedRegion); // show a preview with selection rect
        break;

    case eTools::Selection:
        m_mapScene.setSelectRect(adjustedRegion);
        break;

    case eTools::Paste:
        break;

    default:
        break;
    }
}

void MapTools::useTool(const QRect& clickingRegion)
{
    QRect adjustedRegion = adjustOnGrid(clickingRegion);

    switch (m_currMode) {
    case eTools::Pen:
        break;

    case eTools::Eraser:
        // Hide preview
        m_mapScene.setSelectRect(QRect());
        // and actually erase
        if (m_currLayerType == eLayerType::Visible)
            eraseVisible(adjustedRegion);
        else if (m_currLayerType == eLayerType::Blocking)
            eraseBlocking(adjustedRegion);

        break;

    case eTools::Selection:
        m_mapScene.setSelectRect(adjustedRegion);
        break;

    case eTools::Paste:
        break;

    default:
        break;
    }
}
