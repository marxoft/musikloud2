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

#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>

class ItemMetaData;

class ItemMetaDataRole
{

public:
    explicit ItemMetaDataRole(const QString &name, int role);
    
    QString name;
    int role;
};

class MKTreeView : public QTreeView
{
    Q_OBJECT
    
    Q_PROPERTY(bool itemMetaDataEnabled READ itemMetaDataEnabled WRITE setItemMetaDataEnabled)
    Q_PROPERTY(int itemMetaDataThumbnailRole READ itemMetaDataThumbnailRole WRITE setItemMetaDataThumbnailRole)

public:
    explicit MKTreeView(QWidget *parent = 0);
    
    bool itemMetaDataEnabled() const;
    void setItemMetaDataEnabled(bool enabled);
    
    int itemMetaDataThumbnailRole() const;
    void setItemMetaDataThumbnailRole(int role);
    
    QList<ItemMetaDataRole> itemMetaDataTextRoles() const;
    void setItemMetaDataTextRoles(const QList<ItemMetaDataRole> &roles);

private:
    virtual bool viewportEvent(QEvent *event);

private:
    ItemMetaData *m_itemMetaData;
    
    bool m_itemMetaDataEnabled;
    
    int m_itemMetaDataThumbnailRole;
    
    QList<ItemMetaDataRole> m_itemMetaDataTextRoles;
    
    int m_itemMetaDataRow;
};

#endif // TREEVIEW_H
