#ifndef MAPTOOLS_H
#define MAPTOOLS_H

#include "widgets/chipsetGraphicsScene.hpp"
#include "widgets/mapGraphicsScene.hpp"

//////////////////////////////////////////////////////////////////////////////
//  forward declaration
//////////////////////////////////////////////////////////////////////////////

namespace Ui {
class GeneralWindow;
}

//////////////////////////////////////////////////////////////////////////////
//  MapTools class
//////////////////////////////////////////////////////////////////////////////
namespace Editor {

class Command
{
public:
    virtual ~Command() {}
    virtual void execute() = 0;
    virtual void undo()    = 0;
};

class MapTools : public QObject
{
    Q_OBJECT
    friend class Command;

public:
    enum class eTools
    {
        Pen,
        Eraser,
        Selection,
        Paste,
    };
    enum class eCopyCut
    {
        Copy,
        Cut,
    };

    explicit MapTools(const ChipsetGraphicsScene&, MapGraphicsScene&, Ui::GeneralWindow&);

    void clear();

    void setActiveLayer(LayerGraphicItems&);
    void setActiveLayer(LayerBlockingItems&);

    void setTool(eTools tool);

    void setGrid(bool gridVisible = true);
    void updateGridDisplay();

    void previewTool(const QRect& clickingRegion);
    void useTool(const QRect& clickingRegion);

    void copyCut(eCopyCut);

    void undo();
    void redo();

signals:
    void modificationDone();

private:
    void resetTools();
    void resetLayerLink();

    QPoint adjustOnGrid(const QPoint& pxCoords);
    QRect adjustOnGrid(const QRect& rawRect);
    void forceInScene(QPoint& point); // set the point in the scene if it's out

    QPixmap previewVisible(const QRect&);

    void drawBlocking(const QRect&);
    void drawVisible(const QRect&);

    void eraseBlocking(const QRect&);
    void eraseVisible(const QRect&);

    void paste(const QPoint&);

    void doCommand(std::unique_ptr<Command>&& c);

    struct tVisibleClipboard
    {
        uint16_t width  = 0;
        uint16_t height = 0;
        std::vector<Dummy::Tileaspect> content;
    };

    struct tBlockingClipboard
    {
        uint16_t width  = 0;
        uint16_t height = 0;
        std::vector<bool> content;
    };

    const ChipsetGraphicsScene& m_chipsetScene;
    MapGraphicsScene& m_mapScene;
    Ui::GeneralWindow& m_toolsUI;

    std::vector<std::unique_ptr<Command>> m_commandsHistory; // is reset each time we change the active layer
    size_t m_nbCommandsValid = 0;

    eTools m_currMode                = eTools::Selection;
    eLayerType m_currLayerType       = eLayerType::None;
    LayerGraphicItems* m_visLayer    = nullptr;
    LayerBlockingItems* m_blockLayer = nullptr;
    tVisibleClipboard m_visibleClipboard;
    tBlockingClipboard m_blockingClipboard;

    uint16_t m_uiLayerW   = 0;
    uint16_t m_uiLayerH   = 0;
    uint32_t m_uiGridStep = 0;

    //////////////
    // Command history

    class CommandPaint : public Command
    {
    public:
        CommandPaint(MapTools& parent, QPoint&& pxCoord, tVisibleClipboard&& clip);
        void execute() override;
        void undo() override;

    private:
        MapTools& m_parent;
        QPoint m_position;
        tVisibleClipboard m_toDraw;
        tVisibleClipboard m_replacedTiles;
    };

    class CommandPaintBlocking : public Command
    {
    public:
        CommandPaintBlocking(MapTools& parent, QPoint&& pxCoord, tBlockingClipboard&& clip);
        void execute() override;
        void undo() override;

    private:
        MapTools& m_parent;
        QPoint m_position;
        tBlockingClipboard m_toDraw;
        tBlockingClipboard m_replacedTiles;
    };
};
} // namespace Editor

#endif // MAPTOOLS_H
