#ifndef GRAPHICLAYER_H
#define GRAPHICLAYER_H

#include <QGraphicsItemGroup>

#include "dummyrpg/layer.hpp"
#include "utils/definitions.hpp"

//////////////////////////////////////////////////////////////////////////////
//  forward declaration
//////////////////////////////////////////////////////////////////////////////

namespace Editor {

//////////////////////////////////////////////////////////////////////////////
//  GraphicLayer class
//////////////////////////////////////////////////////////////////////////////

class MapSceneLayer : public QObject
{
public:
    explicit MapSceneLayer(uint8_t floorIdx, uint8_t layerIdx, int zIndex);

    QGraphicsItemGroup* graphicItems() { return m_items; }
    void clear();
    void setVisibility(bool);
    bool isThisFloor(uint8_t floorIdx) const;
    bool isThisLayer(uint8_t floorIdx, uint8_t layerIdx) const;

protected:
    std::vector<QGraphicsItem*>& indexedItems() { return m_indexedItems; }

private:
    uint8_t m_floorIdx;
    uint8_t m_layerIdx;
    QGraphicsItemGroup* m_items = new QGraphicsItemGroup();
    std::vector<QGraphicsItem*> m_indexedItems;
};

//////////////////////////////////////////////////////////////////////////////

class LayerGraphicItems : public MapSceneLayer
{
public:
    explicit LayerGraphicItems(Dummy::GraphicLayer& layer, const std::vector<QPixmap>& chipses,
                               const std::vector<Dummy::chip_id>& chipsetIds, uint8_t floorIdx, uint8_t layerIdx,
                               int zIndex);
    void setTile(Dummy::Coord, Dummy::Tileaspect);
    const Dummy::GraphicLayer& layer();

private:
    size_t idxOfId(Dummy::chip_id);
    Dummy::GraphicLayer& m_graphicLayer;
    const std::vector<QPixmap> m_chipsets;
    const std::vector<Dummy::chip_id> m_chipsetIds;
};

//////////////////////////////////////////////////////////////////////////////

class LayerBlockingItems : public MapSceneLayer
{
public:
    explicit LayerBlockingItems(Dummy::BlockingLayer& layer, uint8_t floorIdx, uint8_t layerIdx, int zIndex);

    void toogleTile(Dummy::Coord);
    void setTile(Dummy::Coord, bool);
    const Dummy::BlockingLayer& layer();

private:
    Dummy::BlockingLayer& m_blockingLayer;
};

} // namespace Editor

#endif // GRAPHICLAYER_H
