#pragma once

#include "abstractmapleveltreeitem.hpp"

class EditorLevel;
class EditorMap;

namespace Misc {
class MapLevelTreeItem : public AbstractMapLevelTreeItem {
public:
    MapLevelTreeItem(EditorLevel&);
    QVariant data(int role = Qt::UserRole + 1) const override;
    virtual void toggle() override;
private:
    EditorLevel& m_editorLevel;
};
} // namespace Misc
