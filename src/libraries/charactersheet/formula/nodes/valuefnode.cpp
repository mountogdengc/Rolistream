/***************************************************************************
 *   Copyright (C) 2016 by Renaud Guezennec                                *
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
#include "charactersheet_formula/nodes/valuefnode.h"
namespace Formula
{
ValueFNode::ValueFNode() {}

bool ValueFNode::run(FormulaNode* previous)
{
    Q_UNUSED(previous)
    if(0 != m_next)
    {
        m_next->run(this);
    }
    return true;
}
void ValueFNode::setValue(QVariant var)
{
    m_value= var;
}

int ValueFNode::getPriority()
{
    int priority= 0;
    if(nullptr != m_next)
    {
        priority= m_next->getPriority();
    }
    return priority;
}

QVariant ValueFNode::getResult()
{
    return m_value;
}
} // namespace Formula
