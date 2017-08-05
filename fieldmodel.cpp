/***************************************************************************
* Copyright (C) 2014 by Renaud Guezennec                                   *
* http://www.rolisteam.org/                                                *
*                                                                          *
*  This file is part of rcse                                               *
*                                                                          *
* rcse is free software; you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* rcse is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program; if not, write to the                            *
* Free Software Foundation, Inc.,                                          *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
***************************************************************************/
#include "fieldmodel.h"

#include <QDebug>
#include <QJsonArray>
#include <QGraphicsScene>

#include "canvas.h"

//////////////////////////////
//Column
/////////////////////////////
Column::Column(QString name,  CharacterSheetItem::ColumnId pos)
    : m_name(name),m_pos(pos)
{

}

QString Column::getName() const
{
    return m_name;
}

void Column::setName(const QString &name)
{
    m_name = name;
}
CharacterSheetItem::ColumnId Column::getPos() const
{
    return m_pos;
}

void Column::setPos(const CharacterSheetItem::ColumnId &pos)
{
    m_pos = pos;
}


//////////////////////////////
//FieldModel
/////////////////////////////
FieldModel::FieldModel(QObject *parent) : QAbstractItemModel(parent)
{
    m_colunm << new Column(tr("Id"),CharacterSheetItem::ID)
             << new Column(tr("Label"),CharacterSheetItem::LABEL)
             << new Column(tr("Value"),CharacterSheetItem::VALUE)
             << new Column(tr("Possible Values"),CharacterSheetItem::VALUES)
             << new Column(tr("Type"),CharacterSheetItem::TYPE)
             << new Column(tr("x"),CharacterSheetItem::X)
             << new Column(tr("y"),CharacterSheetItem::Y)
             << new Column(tr("Width"),CharacterSheetItem::WIDTH)
             << new Column(tr("Height"),CharacterSheetItem::HEIGHT)
             << new Column(tr("Font Adaptation"),CharacterSheetItem::CLIPPED)
             << new Column(tr("Font"),CharacterSheetItem::FONT)
             << new Column(tr("Text-align"),CharacterSheetItem::TEXT_ALIGN)
             << new Column(tr("Text Color"),CharacterSheetItem::TEXTCOLOR)
             << new Column(tr("Bg Color"),CharacterSheetItem::BGCOLOR)
             << new Column(tr("Border"),CharacterSheetItem::BORDER)
             << new Column(tr("Page"),CharacterSheetItem::PAGE);

    m_alignList << tr("TopRight")
           << tr("TopMiddle")
           << tr("TopLeft")
           << tr("CenterRight")
           << tr("CenterMiddle")
           << tr("CenterLeft")
           << tr("BottomRight")
           << tr("BottomMiddle")
           << tr("BottomLeft");

    m_rootSection = new Section();
}

QVariant FieldModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if((role == Qt::DisplayRole)||(Qt::EditRole == role))
    {
        CharacterSheetItem* item = static_cast<CharacterSheetItem*>(index.internalPointer());
        if(NULL!=item)
        {
            QVariant var = item->getValueFrom(m_colunm[index.column()]->getPos(),role);
            if((index.column() == CharacterSheetItem::TEXT_ALIGN)&&(Qt::DisplayRole == role))
            {
                if((var.toInt() >= 0)&&(var.toInt() < m_alignList.size()))
                {
                    var = m_alignList.at(var.toInt());
                }
            }


            return var;
        }
    }
    if((role == Qt::BackgroundRole )&&((index.column() == CharacterSheetItem::BGCOLOR)||(index.column() == CharacterSheetItem::TEXTCOLOR)))
    {
        CharacterSheetItem* item = static_cast<CharacterSheetItem*>(index.internalPointer());
        if(NULL!=item)
        {
            QVariant var = item->getValueFrom(m_colunm[index.column()]->getPos(),Qt::EditRole);
            return var;

        }
    }
    if((Qt::FontRole==role)&&(index.column() == CharacterSheetItem::FONT))
    {
            CharacterSheetItem* item = static_cast<CharacterSheetItem*>(index.internalPointer());
            if(NULL!=item)
            {
                QVariant var = item->getValueFrom(m_colunm[index.column()]->getPos(),Qt::DisplayRole);
                QFont font;
                font.fromString(var.toString());
                return font;
            }
    }
    return QVariant();
}

QModelIndex FieldModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row<0)
        return QModelIndex();

    CharacterSheetItem* parentItem = NULL;

    // qDebug()<< "Index session " <<row << column << parent;
    if (!parent.isValid())
        parentItem = m_rootSection;
    else
        parentItem = static_cast<CharacterSheetItem*>(parent.internalPointer());

    CharacterSheetItem* childItem = parentItem->getChildAt(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex FieldModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    CharacterSheetItem *childItem = static_cast<CharacterSheetItem*>(child.internalPointer());
    CharacterSheetItem *parentItem = childItem->getParent();

    if (parentItem == m_rootSection)
        return QModelIndex();

    CharacterSheetItem* grandParent = parentItem->getParent();

    return createIndex(grandParent->indexOfChild(parentItem), 0, parentItem);
}

int FieldModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return m_rootSection->getChildrenCount();

    CharacterSheetItem *childItem = static_cast<CharacterSheetItem*>(parent.internalPointer());
    if (childItem)
        return childItem->getChildrenCount();
    else
        return 0;

}

int FieldModel::columnCount(const QModelIndex &parent) const
{
    return m_colunm.count();
}

QVariant FieldModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((role==Qt::DisplayRole)&&(orientation == Qt::Horizontal))
    {
        return m_colunm[section]->getName();
    }
    else
    {
        return QVariant();
    }
}

bool FieldModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    if(Qt::EditRole == role)
    {
        CharacterSheetItem* item = static_cast<CharacterSheetItem*>(index.internalPointer());

        if(NULL!=item)
        {
            item->setValueFrom(m_colunm[index.column()]->getPos(),value);
            emit valuesChanged(item->getValueFrom(CharacterSheetItem::ID,Qt::DisplayRole).toString(),value.toString());
            emit modelChanged();
            return true;
        }
    }
    return false;
}

void FieldModel::appendField(CSItem *f)
{
    beginInsertRows(QModelIndex(),m_rootSection->getChildrenCount(),m_rootSection->getChildrenCount());
    m_rootSection->appendChild(f);
    connect(f,SIGNAL(updateNeeded(CSItem*)),this,SLOT(updateItem(CSItem*)));
    endInsertRows();
    emit modelChanged();
}

Qt::ItemFlags FieldModel::flags ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());
    if(m_colunm[index.column()]->getPos() == CharacterSheetItem::ID)
    {
       return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    }
    else if(m_colunm[index.column()]->getPos() == CharacterSheetItem::TYPE)
    {
        QVariant var = index.data();
        if(var.toInt() == Field::SELECT)
        {
           return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
        else
        {
            return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
        }
    }
    else if(!childItem->mayHaveChildren())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable /*| Qt::ItemIsUserCheckable */;
    }
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable /*| Qt::ItemIsUserCheckable */;

}
void FieldModel::generateQML(QTextStream& out,CharacterSheetItem::QMLSection sec)
{
    m_rootSection->generateQML(out,sec);
}

QString FieldModel::getValue(const QString &key)
{
    return key;
}

QList<CharacterSheetItem *> FieldModel::children()
{
   QList<CharacterSheetItem *> result;
   for(int i = 0;i<m_rootSection->getChildrenCount();++i)
   {
    result.append(m_rootSection->getChildAt(i));
   }
   return result;
}

void FieldModel::removePageId(int id)
{
    Section* parentSection = m_rootSection;




    for(int i = m_rootSection->getChildrenCount()-1;i>=0;--i)
    {
      auto child = m_rootSection->getChildAt(i);
      if(id == child->getPage())
      {
          beginRemoveRows(createIndex(parentSection->indexOfChild(child),0,parentSection),
                          parentSection->indexOfChild(child),
                          parentSection->indexOfChild(child));

          parentSection->removeChild(child);
          endRemoveRows();
      }
    }
    emit modelChanged();
}
void FieldModel::updateItem(CSItem* item)
{
    int ind = m_rootSection->indexOfChild(item);
    if(ind>=0)
    {
        emit dataChanged(createIndex(ind,0,item),createIndex(ind,m_colunm.size(),item));
        emit modelChanged();
    }
    else
    {
        CharacterSheetItem* parent = item->getParent();
        QList<CharacterSheetItem*> list;
        while(parent!=NULL)
        {
            list.prepend(parent);
            parent = parent->getParent();
        }


        QModelIndex first;
        QModelIndex second;
        int i=0;
        foreach(CharacterSheetItem* itemtmp, list)
        {
            CharacterSheetItem* next = NULL;
            if(i+1>list.size())
            {
                next = list[++i];
            }
            else
            {
                next = item;
            }

            if(itemtmp==m_rootSection)
            {
                first = index(itemtmp->indexOfChild(next),0,first);
                second = index(itemtmp->indexOfChild(next),m_colunm.size(),second);
            }
        }
        emit dataChanged(first,second);
        emit modelChanged();
    }
}

Section *FieldModel::getRootSection() const
{
    return m_rootSection;
}

void FieldModel::setRootSection(Section *rootSection)
{
    m_rootSection = rootSection;
}
void FieldModel::save(QJsonObject& json,bool exp)
{
    m_rootSection->save(json,exp);
}

void FieldModel::load(QJsonObject &json,QList<Canvas*> scene)
{
    QList<QGraphicsScene*> list;
    for(auto canvas : scene)
    {
        list << canvas;
    }
    beginResetModel();
    m_rootSection->load(json,list);
    endResetModel();
}
void FieldModel::removeItem(QModelIndex& index)
{
    if(index.isValid())
    {
        CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());
        Section* parentSection =NULL;
        if(index.parent().isValid())
        {
            CharacterSheetItem* parentItem = static_cast<CharacterSheetItem*>(index.internalPointer());
            parentSection = dynamic_cast<Section*>(parentItem);
        }
        else
        {
            parentSection = m_rootSection;
        }

        if(NULL==parentSection)
        {
            return;
        }
        beginRemoveRows(index.parent(),
                        parentSection->indexOfChild(childItem),
                        parentSection->indexOfChild(childItem));

        parentSection->removeChild(childItem);

        endRemoveRows();



    emit modelChanged();
    }
}
void FieldModel::clearModel()
{
    beginResetModel();
    m_rootSection->removeAll();
    endResetModel();
}

void FieldModel::setValueForAll(QModelIndex& index)
{
    if(index.isValid())
    {
        CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());
        m_rootSection->setValueForAll(childItem,m_colunm[index.column()]->getPos());
    }
}

void FieldModel::resetAllId()
{
    beginResetModel();
    int i = 0;
    m_rootSection->resetAllId(i);
    qDebug() << "i:" << i;
    Field::setCount(i);
    endResetModel();
}

