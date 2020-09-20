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
#include "filteredplayermodel.h"
#include "userlist/playermodel.h"

namespace InstantMessaging
{
FilteredPlayerModel::FilteredPlayerModel(const QStringList& list, QObject* parent)
    : QSortFilterProxyModel(parent), m_participants(list)
{
}

int FilteredPlayerModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_participants.size();
}

QStringList FilteredPlayerModel::recipiantIds() const
{
    return m_participants;
}

bool FilteredPlayerModel::hasRecipiant(const QString& uuid)
{
    return m_participants.contains(uuid);
}

QString FilteredPlayerModel::recipiantName(const QString& uuid)
{
    for(int i= 0; i < rowCount(); ++i)
    {
        auto indexModel= index(i, 0);
        auto uuid= indexModel.data().toString();
    }
}

bool FilteredPlayerModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    auto idx= sourceModel()->index(sourceRow, 0, sourceParent);
    auto uuid= idx.data().toString();
    return m_participants.contains(uuid);
}
} // namespace InstantMessaging
