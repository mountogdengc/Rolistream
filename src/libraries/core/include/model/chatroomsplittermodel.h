/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#ifndef CHATROOMSPLITTERMODEL_H
#define CHATROOMSPLITTERMODEL_H

#include <QAbstractListModel>
#include <core_global.h>
#include <memory>
#include <vector>
namespace InstantMessaging
{
class FilterInstantMessagingModel;
class InstantMessagingModel;
class CORE_EXPORT ChatroomSplitterModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role
    {
        FilterModelRole= Qt::UserRole + 1,
        UuidRole
    };
    explicit ChatroomSplitterModel(QObject* parent= nullptr);

    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;
    void addFilterModel(InstantMessaging::InstantMessagingModel* sourceModel, QStringList list= QStringList(),
                        bool all= true);

private:
    std::vector<std::unique_ptr<FilterInstantMessagingModel>> m_filterModels;
};
} // namespace InstantMessaging

#endif // CHATROOMSPLITTERMODEL_H
