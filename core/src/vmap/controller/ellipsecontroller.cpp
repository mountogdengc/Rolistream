/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "ellipsecontroller.h"

#include <QVariant>
namespace vmap
{
EllipseController::EllipseController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl,
                                     QObject* parent)
    : VisualItemController(ctrl, parent)
{
    m_color= params.at(QStringLiteral("color")).value<QColor>();
    m_filled= (params.at(QStringLiteral("tool")).value<Core::SelectableTool>() == Core::SelectableTool::FILLEDELLIPSE);
    m_penWidth= static_cast<quint16>(params.at(QStringLiteral("penWidth")).toInt());
    m_pos= params.at(QStringLiteral("position")).toPointF();
}

bool EllipseController::filled() const
{
    return m_filled;
}

QColor EllipseController::color() const
{
    return m_color;
}

quint16 EllipseController::penWidth() const
{
    return m_penWidth;
}

qreal EllipseController::rx() const
{
    return m_rx;
}

qreal EllipseController::ry() const
{
    return m_ry;
}

void EllipseController::aboutToBeRemoved()
{
    emit removeItem();
}

void EllipseController::endGeometryChange() {}

void EllipseController::setColor(QColor color)
{
    if(color == m_color)
        return;
    m_color= color;
    emit colorChanged();
}

void EllipseController::setCorner(const QPointF& move, int corner)
{
    if(move.isNull())
        return;

    auto rx= m_rx;
    auto ry= m_ry;
    switch(corner)
    {
    case 0:
        rx+= move.x();
        break;
    case 1:
        ry+= move.y();
        break;
    case 2:
        rx+= move.x();
        ry+= move.y();
        break;
    }
    setRx(rx);
    setRy(ry);
}

void EllipseController::setRx(qreal rx)
{
    if(qFuzzyCompare(rx, m_rx) || qFuzzyIsNull(rx))
        return;
    m_rx= rx;
    emit rxChanged();
}

void EllipseController::setRy(qreal ry)
{
    if(qFuzzyCompare(ry, m_ry) || qFuzzyIsNull(ry))
        return;
    m_ry= ry;
    emit ryChanged();
}
} // namespace vmap
