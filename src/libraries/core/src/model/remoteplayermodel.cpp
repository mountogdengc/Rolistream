/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "model/remoteplayermodel.h"

#include "model/playermodel.h"

RemotePlayerModel::RemotePlayerModel(PlayerModel* model) : m_sourceModel(model)
{
    setSourceModel(model);
}

PlayerModel* RemotePlayerModel::sourceModel() const
{
    return m_sourceModel;
}

bool RemotePlayerModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    auto idx= sourceModel()->index(source_row, 0, source_parent);
    return !idx.data(PlayerModel::LocalRole).toBool();
}
