/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "nowplayingdelegate.h"
#include "trackmodel.h"
#include <QPainter>
#include <QApplication>

NowPlayingDelegate::NowPlayingDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void NowPlayingDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if ((option.state) & (QStyle::State_Selected)) {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundPressed.png"));
    }
    else {
        painter->drawImage(option.rect, QImage("/etc/hildon/theme/images/TouchListBackgroundNormal.png"));
    }

    QRect textRect = option.rect;
    textRect.setLeft(textRect.left() + 8);
    textRect.setRight(textRect.right() - 8);
    textRect.setTop(textRect.top() + 8);
    textRect.setBottom(textRect.bottom() - 8);
    
    QFontMetrics fm = painter->fontMetrics();
    
    QString duration = index.data(TrackModel::DurationStringRole).toString();
    QString title = fm.elidedText(index.data(TrackModel::TitleRole).toString(), Qt::ElideRight,
                                  textRect.width() - fm.width(duration) - 8);
    
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignTop, title);
    painter->drawText(textRect, Qt::AlignRight | Qt::AlignTop, duration);
        
    QFont font;
    font.setPointSize(13);
    
    QString artist = index.data(TrackModel::ArtistRole).toString();
    
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
