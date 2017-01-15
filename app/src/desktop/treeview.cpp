/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "treeview.h"
#include "itemmetadata.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QHelpEvent>

ItemMetaDataRole::ItemMetaDataRole(const QString &name, int role)
{
    this->name = name;
    this->role = role;
}

MKTreeView::MKTreeView(QWidget *parent) :
    QTreeView(parent),
    m_itemMetaData(0),
    m_itemMetaDataEnabled(false),
    m_itemMetaDataThumbnailRole(Qt::DecorationRole),
    m_itemMetaDataRow(-1)
{
}

bool MKTreeView::itemMetaDataEnabled() const {
    return m_itemMetaDataEnabled;
}

void MKTreeView::setItemMetaDataEnabled(bool enabled) {
    m_itemMetaDataEnabled = enabled;
    viewport()->setMouseTracking(enabled);
}

int MKTreeView::itemMetaDataThumbnailRole() const {
    return m_itemMetaDataThumbnailRole;
}

void MKTreeView::setItemMetaDataThumbnailRole(int role) {
    m_itemMetaDataThumbnailRole = role;
}

QList<ItemMetaDataRole> MKTreeView::itemMetaDataTextRoles() const {
    return m_itemMetaDataTextRoles;
}

void MKTreeView::setItemMetaDataTextRoles(const QList<ItemMetaDataRole> &roles) {
    m_itemMetaDataTextRoles = roles;
}

bool MKTreeView::viewportEvent(QEvent *event) {
    if ((event->type() == QEvent::ToolTip) && (itemMetaDataEnabled())) {
        if (QHelpEvent *he = static_cast<QHelpEvent*>(event)) {
            const QModelIndex index = indexAt(he->pos());
            
            if ((index.isValid()) && ((!m_itemMetaData) || (!m_itemMetaData->isVisible()))) {
                if (!m_itemMetaData) {
                    m_itemMetaData = new ItemMetaData(this);
                    m_itemMetaData->setWindowFlags(Qt::ToolTip);
                    m_itemMetaData->setThumbnailSize(QSize(96, 96));
                }
                
                m_itemMetaDataRow = index.row();
                QList<ItemMetaDataValue> metaData;
                
                for (int i = 0; i < m_itemMetaDataTextRoles.size(); i++) {
                    metaData << ItemMetaDataValue(m_itemMetaDataTextRoles.at(i).name,
                                                  index.data(m_itemMetaDataTextRoles.at(i).role));
                }
                
                m_itemMetaData->setMetaData(metaData);
                m_itemMetaData->setThumbnailSource(index.data(itemMetaDataThumbnailRole()).toString());
                
                const QRect rect = QApplication::desktop()->availableGeometry(m_itemMetaData);
                QPoint pos = he->globalPos();
                pos.setX(pos.x() + 1);
                pos.setY(pos.y() + 1);
                
                if (pos.x() + m_itemMetaData->width() > rect.right()) {
                    pos.setX(pos.x() - m_itemMetaData->width() - 2);
                }
                
                if (pos.y() + m_itemMetaData->height() > rect.bottom()) {
                    pos.setY(pos.y() - m_itemMetaData->height() - 2);
                }
                
                m_itemMetaData->move(pos);
                m_itemMetaData->show();
                return true;
            }
        }
    }
    else if ((event->type() == QEvent::MouseMove) && (m_itemMetaData)) {
        if (QMouseEvent *me = static_cast<QMouseEvent*>(event)) {
            const QModelIndex index = indexAt(me->pos());
            
            if ((!index.isValid()) || (index.row() != m_itemMetaDataRow)) {
                m_itemMetaData->hide();
            }
        }
    }
    else if (((event->type() == QEvent::MouseButtonPress) || (event->type() == QEvent::Leave)
              || (event->type() == QEvent::Hide) || (event->type() == QEvent::Close)) && (m_itemMetaData)) {
        m_itemMetaData->hide();
    }
    
    return QTreeView::viewportEvent(event);
}
