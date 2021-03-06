/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MINDMAPCONTROLLER_H
#define MINDMAPCONTROLLER_H

#include "mediacontrollerbase.h"

#include <QAbstractItemModel>
#include <QPointer>
#include <QRectF>
#include <QUndoStack>
#include <core_global.h>
#include <memory>

#include "mindmap/controller/selectioncontroller.h"
#include "mindmap/model/imagemodel.h"
#include "model/playermodel.h"
#include "model/remoteplayermodel.h"

class QQuickImageProvider;
class MindMapUpdater;

namespace mindmap
{
class BoxModel;
class LinkModel;
class Link;
class MindNode;
class SpacingController;
class SelectionController;
class NodeStyleModel;
class NodeStyle;
class NodeImageProvider;
} // namespace mindmap

class CORE_EXPORT MindMapController : public MediaControllerBase
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* nodeModel READ nodeModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* linkModel READ linkModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* styleModel READ styleModel CONSTANT)
    Q_PROPERTY(RemotePlayerModel* remotePlayerModel READ remotePlayerModel CONSTANT)
    Q_PROPERTY(PlayerModel* playerModel READ playerModel CONSTANT)
    Q_PROPERTY(int defaultStyleIndex READ defaultStyleIndex WRITE setDefaultStyleIndex NOTIFY defaultStyleIndexChanged)
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(bool spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(mindmap::SelectionController* selectionCtrl READ selectionController CONSTANT)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(QString errorMsg READ errorMsg WRITE setErrorMsg NOTIFY errorMsgChanged)
    Q_PROPERTY(QRectF contentRect READ contentRect NOTIFY contentRectChanged)
    Q_PROPERTY(bool readWrite READ readWrite NOTIFY sharingToAllChanged)
    Q_PROPERTY(Core::SharingPermission sharingToAll READ sharingToAll NOTIFY sharingToAllChanged)
    Q_PROPERTY(mindmap::ImageModel* imageModel READ imageModel CONSTANT)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged)
public:
    enum SubItemType
    {
        Node,
        Link,
        Package
    };
    Q_ENUM(SubItemType)
    explicit MindMapController(const QString& id, QObject* parent= nullptr);
    ~MindMapController();

    QAbstractItemModel* nodeModel() const;
    QAbstractItemModel* linkModel() const;
    QAbstractItemModel* styleModel() const;
    RemotePlayerModel* remotePlayerModel() const;
    PlayerModel* playerModel() const;
    mindmap::ImageModel* imageModel() const;

    mindmap::SelectionController* selectionController() const;
    const QString& filename() const;
    const QString& errorMsg() const;
    QRectF contentRect() const;

    bool readWrite() const;

    bool spacing() const;
    bool canRedo() const;
    bool canUndo() const;
    bool hasSelection() const;

    int defaultStyleIndex() const;
    Core::SharingPermission sharingToAll() const;

    void addNode(QList<mindmap::MindNode*> nodes, bool network= false);
    mindmap::Link* linkFromId(const QString& id) const;
    mindmap::MindNode* nodeFromId(const QString& id) const;
    void createLink(const QString& id, const QString& id2);
    void addLink(const QList<mindmap::Link*>& link, bool network= false);
    bool pasteData(const QMimeData& mimeData) override;

    QObject* subItem(const QString& id, SubItemType type);

    static void setRemotePlayerModel(RemotePlayerModel* model);
    static void setMindMapUpdater(MindMapUpdater* updater);

    // testing only
    mindmap::SpacingController* spacingController() const;
signals:
    void filenameChanged();
    void spacingChanged();
    void canRedoChanged();
    void canUndoChanged();
    void errorMsgChanged();
    void defaultStyleIndexChanged();
    void contentRectChanged();
    void sharingToAllChanged(Core::SharingPermission newPerm, Core::SharingPermission formerPerm);
    void recipientsChanged();
    void hasSelectionChanged();

    void openMindMapTo(QString id);
    void closeMindMapTo(QString id);
    void permissionChangedForUser(QString id, Core::SharingPermission perm);

public slots:
    void saveFile();
    void loadFile();
    void setFilename(const QString& path);
    void setSpacing(bool b);
    void redo();
    void undo();
    void setErrorMsg(const QString& msg);
    void importFile(const QString& path);
    void setDefaultStyleIndex(int indx);
    void setSharingToAll(int perm);
    void setPermissionForUser(const QString& userId, int perm);
    void generateTree();
    // void setReadWrite(bool b);

    void addBox(const QString& idparent);
    void addCharacterBox(const QString& idparent, const QString& name, const QString& url, const QColor& color);
    void reparenting(mindmap::MindNode* parent, const QString& id);
    void removeSelection();
    mindmap::NodeStyle* getStyle(int index) const;

    void openImage(const QString& id, const QUrl& path);
    void removeImage(const QString& id);
    void removeLink(const QStringList& id);
    void removeNode(const QStringList& id);

protected:
    void clearData();

private:
    QString m_filename;
    QString m_errorMsg;
    std::unique_ptr<mindmap::SpacingController> m_spacingController;
    std::unique_ptr<mindmap::SelectionController> m_selectionController;
    std::unique_ptr<mindmap::LinkModel> m_linkModel;
    std::unique_ptr<mindmap::BoxModel> m_nodeModel;
    std::unique_ptr<mindmap::NodeStyleModel> m_styleModel;
    std::unique_ptr<mindmap::ImageModel> m_imageModel;
    static QPointer<RemotePlayerModel> m_remotePlayerModel;
    static QPointer<MindMapUpdater> m_updater;
    QThread* m_spacing= nullptr;
    QUndoStack m_stack;
    // bool m_readWrite= false;

    Core::SharingPermission m_sharingToAll= Core::SharingPermission::None;
    QHash<QString, Core::SharingPermission> m_permissions;
};

#endif // MINDMAPCONTROLLER_H
