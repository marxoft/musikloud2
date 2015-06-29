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

#include "trackdelegate.h"
#include "drawing.h"
#include "imagecache.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

TrackDelegate::TrackDelegate(ImageCache *cache, int artistRole, int dateRole, int durationRole, int thumbnailRole,
                             int titleRole, QObject *parent) :
    QStyledItemDelegate(parent),
    m_cache(cache),
    m_artistRole(artistRole),
    m_dateRole(dateRole),
    m_durationRole(durationRole),
    m_thumbnailRole(thumbnailRole),
    m_titleRole(titleRole),
    m_pressedRow(-1)
{
}

bool TrackDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option,
                                const QModelIndex &index) {

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

        QRect imageRect = option.rect;
        imageRect.setLeft(imageRect.left() + 8);
        imageRect.setWidth(imageRect.height());

        if (imageRect.contains(mouse->pos())) {
            m_pressedRow = index.row();
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        if (m_pressedRow == index.row()) {
            QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

            QRect imageRect = option.rect;
            imageRect.setLeft(imageRect.left() + 8);
            imageRect.setWidth(imageRect.height());

            if (imageRect.contains(mouse->pos())) {
                emit thumbnailClicked(index);
                m_pressedRow = -1;
                return true;
            }
        }
        
        m_pressedRow = -1;
    }

    return false;
}

void TrackDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (((option.state) & (QStyle::State_Selected))  && (m_pressedRow != index.row())) {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundPressed.png"));
    }
    else {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundNormal.png"));
    }
   
    QImage image = m_cache->image(index.data(m_thumbnailRole).toString(), QSize(64, 64));
    
    if (image.isNull()) {
        image = QImage("/usr/share/icons/hicolor/64x64/hildon/mediaplayer_default_album.png");
    }
    
    QRect imageRect = option.rect;
    imageRect.setLeft(imageRect.left() + 8);
    imageRect.setWidth(imageRect.height());
    
    drawCenteredImage(painter, imageRect, image);
    
    QRect textRect = option.rect;
    textRect.setLeft(imageRect.right() + 8);
    textRect.setRight(textRect.right() - 8);
    textRect.setTop(textRect.top() + 8);
    textRect.setBottom(textRect.bottom() - 8);
    
    QFontMetrics fm = painter->fontMetrics();
    
    QString duration = index.data(m_durationRole).toString();
    QString title = fm.elidedText(index.data(m_titleRole).toString(), Qt::ElideRight,
                                  textRect.width() - fm.width(duration) - 8);
    
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop, title);
    painter->drawText(textRect, Qt::AlignRight | Qt::AlignTop, duration);
        
    QFont font;
    font.setPointSize(13);
    
    QString artist = index.data(m_artistRole).toString();
    
    if (artist.isEmpty()) {
        artist = tr("Unknown artist");
    }
        
    painter->save();
    painter->setFont(font);
    painter->setPen(QApplication::palette().color(QPalette::Mid));
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignBottom,
                      QFontMetrics(font).elidedText(artist, Qt::ElideRight, textRect.width()));
    painter->restore();
}
