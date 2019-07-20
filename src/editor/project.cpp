#include <algorithm>

#include <QDebug>
#include <QDir>
#include <QDomNode>
#include <QFile>
#include <QString>
#include <QTextStream>

#include "editor/map.hpp"
#include "misc/map_tree_model.hpp"

#include "editor/project.hpp"
#include "editor/starting_point.hpp"

namespace Editor {
Project::Project(const std::string& projectFolder)
    : m_coreProject(fs::path(projectFolder)),
      m_mapsModel(nullptr)
{

    // Try to read the "project.xml" file that should be present in folderPath.
    QFile xmlProjectFile(
        (m_coreProject.projectPath() / "project.xml").string().c_str()
    );
    m_domDocument.setContent(&xmlProjectFile);
    xmlProjectFile.close();

    QDomNodeList mapsNodes = m_domDocument
        .documentElement()
        .elementsByTagName("maps");

    if (mapsNodes.length() > 0) {
        m_mapsModel = new Misc::MapTreeModel(mapsNodes.at(0));
    } else {
        // XXX: Throw exception?
    }

    QDomNodeList startingPositionNodes = m_domDocument.documentElement()
        .elementsByTagName("starting_point");

    if (startingPositionNodes.length() > 0)
    {
        const QDomNode& startingPositionNode(startingPositionNodes.at(0));
        const QDomNamedNodeMap& attributes(startingPositionNode.attributes());
        m_startingPoint = std::make_unique<StartingPoint>(
            attributes.namedItem("map").nodeValue().toStdString().c_str(),
            attributes.namedItem("x").nodeValue().toUShort(),
            attributes.namedItem("y").nodeValue().toUShort(),
            attributes.namedItem("floor").nodeValue().toUShort()
        );
    }
}

Project::~Project() {

    delete m_mapsModel;
}

Misc::MapTreeModel* Project::mapsModel() {
    return m_mapsModel;
}

void Project::setStartingPoint(
    const StartingPoint& startingPoint)
{
    m_startingPoint = std::make_unique<StartingPoint>(startingPoint);
}

void Project::create(const QString& folder) {
    _createXmlProjectFile(folder);
    _createFolders(folder);

}

void Project::_createXmlProjectFile(const QString& folder) {
    QFile projectFile(folder + "/" + "project.xml");
    projectFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&projectFile);
    out << _createXmlProjectTree().toString(4);
    projectFile.close();
}

QDomDocument Project::_createXmlProjectTree() {
    QDomDocument doc;
    QDomElement project = doc.createElement("project");
    QDomElement maps = doc.createElement("maps");

    doc.appendChild(project);
    project.appendChild(maps);

    return doc;
}

void Project::_createFolders(const QString& baseFolder) {
    std::vector<QString> folders{"maps", "chipsets", "sounds"};
    std::for_each(folders.begin(), folders.end(),
                  [&baseFolder](const QString& folder) {
        QDir dir(baseFolder + "/" + folder);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        qDebug() << "create folder " << folder;
    });
}

void Project::saveProjectFile() {
    QDomDocument doc;
    QDomElement projectNode = doc.createElement("project");
    QDomElement mapsNode = doc.createElement("maps");

    doc.appendChild(projectNode);
    projectNode.appendChild(mapsNode);

    if (nullptr != m_startingPoint)
    {
        QDomElement startingPointNode = doc.createElement("starting_point");
        startingPointNode.setAttribute("map", m_startingPoint->mapName());
        startingPointNode.setAttribute("x", m_startingPoint->x());
        startingPointNode.setAttribute("y", m_startingPoint->y());
        startingPointNode.setAttribute(
            "floor",
            static_cast<std::uint16_t>(m_startingPoint->floor())
        );
        projectNode.appendChild(startingPointNode);
    }

    _dumpToXmlNode(doc, mapsNode, m_mapsModel->invisibleRootItem());
    QString xmlPath(
        (m_coreProject.projectPath() / "project.xml").string().c_str()
    );

    // XXX: Handle errors eventually.
    QFile file(xmlPath);
    file.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream stream(&file);
    doc.save(stream, 4);
}

void Project::_dumpToXmlNode(QDomDocument& doc,
                             QDomElement& xmlNode,
                             QStandardItem* modelItem) {

    for(int i = 0; i < modelItem->rowCount(); ++i) {
        QStandardItem* mapItem = modelItem->child(i);

        QDomElement mapNode = doc.createElement("map");
        mapNode.setAttribute("name", mapItem->text());

        xmlNode.appendChild(mapNode);

        _dumpToXmlNode(doc, mapNode, mapItem);
    }
}

void Project::cleanMapName(QString& mapName) {
    mapName.replace("/", "");
    mapName.replace("..", "");
}

std::shared_ptr<Misc::MapDocument>
Project::document(const QString& mapName) {
    QString cleantMapname(mapName);
    cleanMapName(cleantMapname);

    if (!m_openedMaps.contains(cleantMapname)) {
        auto map = std::make_shared<Editor::Map>(
            m_coreProject, cleantMapname.toStdString()
        );
        map->load();

        auto mapDocument = std::make_shared<Misc::MapDocument>(
            *this, cleantMapname, map
        );

        m_openedMaps.insert(cleantMapname, mapDocument);
    }
    return m_openedMaps[cleantMapname];
}
} // namespace Editor
