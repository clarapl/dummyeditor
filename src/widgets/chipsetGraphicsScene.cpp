#include "widgets/chipsetGraphicsScene.hpp"

#include <QGraphicsSceneMouseEvent>

#include "utils/definitions.hpp"

ChipsetGraphicsScene::ChipsetGraphicsScene(QObject* parent)
    : QGraphicsScene(parent)
{}

void ChipsetGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (! mouseEvent->buttons().testFlag(Qt::LeftButton) || m_isSelecting)
        return;

    m_isSelecting    = true;
    m_selectionStart = mouseEvent->scenePos().toPoint();

    if (m_selectionStart.x() >= width() || m_selectionStart.x() < 0 || m_selectionStart.y() >= height()
        || m_selectionStart.y() < 0) {
        return;
    }

    // Add a square
    int x = m_selectionStart.x() - (m_selectionStart.x() % CELL_W);
    int y = m_selectionStart.y() - (m_selectionStart.y() % CELL_H);
    setSelectRect(QRect(x, y, CELL_W, CELL_H));
}

void ChipsetGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (! m_isSelecting)
        return;

    QPoint pt = mouseEvent->scenePos().toPoint();

    // normalize selection rectangle in order accept any direction of selection
    QRect realRect = QRect(m_selectionStart, pt).normalized();

    // extend the rectangle to catch the complete border tiles
    realRect.setLeft(realRect.left() - (realRect.left() % CELL_W));
    realRect.setTop(realRect.top() - (realRect.top() % CELL_H));
    realRect.setRight(((realRect.right() / CELL_W) + 1) * CELL_W - 1);
    realRect.setBottom(((realRect.bottom() / CELL_H) + 1) * CELL_H - 1);

    setSelectRect(realRect);
}

void ChipsetGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent*)
{
    m_isSelecting = false;
}

QPixmap ChipsetGraphicsScene::selectionPixmap() const
{
    if (m_currentSelection.isNull())
        return QPixmap();

    return m_chipset.copy(m_currentSelection);
}

void ChipsetGraphicsScene::clear()
{
    m_selectionRectItem.reset();
    m_gridItems.clear();
    QGraphicsScene::clear();
}

void ChipsetGraphicsScene::setGridVisible(bool visible)
{
    if (visible)
        drawGrid();
    else
        m_gridItems.clear();
}

void ChipsetGraphicsScene::setChipset(const std::vector<QString>& chipsetPaths,
                                      const std::vector<Dummy::chip_id>& chipsetIds)
{
    clear();

    // TODO handle several chipsets

    m_chipset = QPixmap(chipsetPaths[0]);
    m_currId  = chipsetIds[0];
    addPixmap(m_chipset);
    setSelectRect(QRect(0, 0, 0, 0));
    drawGrid();
}

void ChipsetGraphicsScene::drawGrid()
{
    m_gridItems.clear();

    QPen pen(Qt::black, 0.5);

    int chipW = m_chipset.width();
    for (int x = 0; x <= chipW; x += CELL_W) {
        QGraphicsItem* item = addLine(x, 0, x, m_chipset.height(), pen);
        item->setZValue(Z_GRID);
        m_gridItems.push_back(std::unique_ptr<QGraphicsItem>(item));
    }

    int chipH = m_chipset.height();
    for (int y = 0; y <= chipH; y += CELL_H) {
        QGraphicsItem* item = addLine(0, y, m_chipset.width(), y, pen);
        item->setZValue(Z_GRID);
        m_gridItems.push_back(std::unique_ptr<QGraphicsItem>(item));
    }
}

void ChipsetGraphicsScene::setSelectRect(const QRect& rect)
{
    m_currentSelection = rect;

    m_selectionRectItem = std::make_unique<QGraphicsRectItem>(rect);
    QBrush brush(QColor(66, 135, 245));
    m_selectionRectItem->setBrush(brush);
    m_selectionRectItem->setOpacity(0.5);
    addItem(m_selectionRectItem.get());
}
