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

#ifndef TRACKDELEGATE_H
#define TRACKDELEGATE_H

#include <QStyledItemDelegate>

class ImageCache;

class TrackDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    
public:
    explicit TrackDelegate(ImageCache *cache, int artistRole, int dateRole, int durationRole, int thumbnailRole,
                           int titleRole, QObject *parent = 0);
    
    bool editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option,
                     const QModelIndex &index);
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
Q_SIGNALS:
    void thumbnailClicked(const QModelIndex &index);
    
private:
    ImageCache *m_cache;
    
    int m_artistRole;
    int m_dateRole;
    int m_durationRole;
    int m_thumbnailRole;
    int m_titleRole;
    
    int m_pressedRow;
};

#endif // TRACKDELEGATE_H
