/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                                      *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef CHANGECOLORITEM_H
#define CHANGECOLORITEM_H

#include <QColor>
#include <QPointer>
#include <QUndoCommand>
#include <core_global.h>
namespace vmap
{
class VisualItemController;
}

class CORE_EXPORT ChangeColorItemCmd : public QUndoCommand
{
public:
    ChangeColorItemCmd(vmap::VisualItemController* item, QColor newColor, QUndoCommand* parent= nullptr);

    void redo() override;
    void undo() override;

private:
    QPointer<vmap::VisualItemController> m_ctrl;
    QColor m_oldColor;
    QColor m_newColor;
};

#endif // CHANGECOLORITEM_H
