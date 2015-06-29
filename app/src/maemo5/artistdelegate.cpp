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

#include "artistdelegate.h"
#include "drawing.h"
#include "imagecache.h"
#include <QPainter>
#include <QApplication>

ArtistDelegate::ArtistDelegate(ImageCache *cache, int nameRole, int thumbnailRole, QObject *parent) :
    QStyledItemDelegate(parent),
    m_cache(cache),
    m_nameRole(nameRole),
    m_thumbnailRole(thumbnailRole)
{
}

void ArtistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if ((option.state) & (QStyle::State_Selected)) {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundPressed.png"));
    }
    else {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundNormal.png"));
    }
   
    QImage image = m_cache->image(index.data(m_thumbnailRole).toString(), QSize(64, 64));
    
    if (image.isNull()) {
        image = QImage("/usr/share/icons/hicolor/64x64/hildon/general_default_avatar.png");
    }
    
    QRect imageRect = option.rect;
    imageRect.setLeft(imageRect.left() + 8);
    imageRect.setWidth(imageRect.height());
    
    drawCenteredImage(painter, imageRect, image);
    
    QRect textRect = option.rect;
    textRect.setLeft(imageRect.right() + 8);
    textRect.setRight(textRect.right() - 8);
        
    painter->drawText(textRect, Qt::AlignVCenter, painter->fontMetrics().elidedText(index.data(m_nameRole).toString(),
                      Qt::ElideRight, textRect.width()));
}
