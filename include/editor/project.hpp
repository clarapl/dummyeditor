#ifndef EDITORPROJECT_H
#define EDITORPROJECT_H

#include <memory>

#include <QDomDocument>

#include <dummy/local/project.hpp>

#include "mapsTreeModel.hpp"

//////////////////////////////////////////////////////////////////////////////
//  forward declaration
//////////////////////////////////////////////////////////////////////////////

class MapDocument;

namespace Editor {
class StartingPoint;

//////////////////////////////////////////////////////////////////////////////
//  Errors
//////////////////////////////////////////////////////////////////////////////

class ProjectError : public std::exception
{};

//////////////////////////////////////////////////////////////////////////////
//  Project class
//////////////////////////////////////////////////////////////////////////////

class Project
{
public:
    Project(const std::string& folder);
    virtual ~Project();

    // Getters
    MapTreeModel* mapsModel();
    const Dummy::Local::Project& coreProject() const { return m_coreProject; }
    std::shared_ptr<MapDocument> document(const QString& mapName);
    QMap<QString, std::shared_ptr<MapDocument>> openedMaps() const;

    // Setters
    void setModified(bool isModified) { m_isModified = isModified; }
    void setStartingPoint(const StartingPoint&);

    void saveProjectFile();
    static void create(const QString&);
    static void cleanMapName(QString& mapName);

private:
    void dumpToXmlNode(QDomDocument& document, QDomElement& xmlNode,
                       QStandardItem* modelItem);

    static QDomDocument createXmlProjectTree();
    static void createXmlProjectFile(const QString&);
    static void createFolders(const QString&);

private:
    Dummy::Local::Project m_coreProject;
    QDomDocument m_domDocument;
    MapTreeModel* m_mapsModel;
    bool m_isModified;
    std::unique_ptr<StartingPoint> m_startingPoint;

    QMap<QString, std::shared_ptr<MapDocument>> m_openedMaps;
};

} // namespace Editor

#endif // EDITORPROJECT_H
