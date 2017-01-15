/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
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

#ifndef ARTISTDELEGATE_H
#define ARTISTDELEGATE_H

#include <QStyledItemDelegate>

class ImageCache;

class ArtistDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    
public:
    explicit ArtistDelegate(ImageCache *cache, int nameRole, int thumbnailRole, QObject *parent = 0);
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        
private:
    ImageCache *m_cache;
    
    int m_nameRole;
    int m_thumbnailRole;    
};

#endif // ARTISTDELEGATE_H
