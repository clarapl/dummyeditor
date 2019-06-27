#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "core/graphic_map.hpp"

#include "editorlayer.hpp"
#include "editorlevel.hpp"

using EditorLevels = std::vector<std::unique_ptr<EditorLevel>>;

class EditorMap : public Dummy::Core::GraphicMap {
public:
    EditorMap(const Dummy::Core::Project&, const std::string&);
    virtual ~EditorMap();

    Dummy::Core::GraphicLayer& firstLayer() {
        //return m_firstLayer;
        return m_mapLevels[0].layers().at(-1);
    }

    Dummy::Core::GraphicLayer& secondLayer() {
        //return m_secondLayer;
        return m_mapLevels[0].layers().at(0);
    }

    Dummy::Core::GraphicLayer& thirdLayer() {
        //return m_thirdLayer;
        return m_mapLevels[0].layers().at(1);
    }

    Dummy::Core::GraphicLayer& fourthLayer() {
        //return m_fourthLayer;
        return m_mapLevels[0].layers().at(2);
    }

    void setChipset(const std::string&);
    void setMusic(const std::string&);
    void reset(std::uint16_t, std::uint16_t);

    void save();
    void resize(std::uint16_t, std::uint16_t);

    std::uint8_t levelsCount() const {
        return m_levelsCount;
    }

    EditorLevel& levelAt(std::uint8_t index) {
        return *m_editorLevels[index];
    }

    EditorLayer& editorLayerAt(std::uint8_t level, std::int8_t position) {
        return m_editorLevels[level]->layerAt(position);
    }

    void load();

private:
    void _saveBlockingLayers();
    void _saveGraphicLayers();

    void _resizeBlockingLayer(std::uint16_t, std::uint16_t);
    void _resizeGraphicLayer(
        Dummy::Core::GraphicLayer&,
        std::uint16_t,
        std::uint16_t
    );

    static void _writeStdString(std::ofstream&, const std::string&);
    void _writeLevel(std::ofstream&, const Dummy::Core::MapLevel&);
    EditorLevels m_editorLevels;


};
