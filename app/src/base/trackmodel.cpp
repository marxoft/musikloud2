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

#include "trackmodel.h"
#include "audioplayer.h"
#include <QFont>

TrackModel::TrackModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_roles[ArtistRole] = "artist";
    m_roles[ArtistIdRole] = "artistId";
    m_roles[DateRole] = "date";
    m_roles[DescriptionRole] = "description";
    m_roles[DownloadableRole] = "downloadable";
    m_roles[DurationRole] = "duration";
    m_roles[DurationStringRole] = "durationString";
    m_roles[FormatRole] = "format";
    m_roles[GenreRole] = "genre";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[PlayCountRole] = "playCount";
    m_roles[ServiceRole] = "service";
    m_roles[SizeRole] = "size";
    m_roles[SizeStringRole] = "sizeString";
    m_roles[StreamUrlRole] = "streamUrl";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
    m_roles[UrlRole] = "url";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> TrackModel::roleNames() const {
    return m_roles;
}
#endif

int TrackModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_items.size();
}

int TrackModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 4;
}

QVariant TrackModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((orientation != Qt::Horizontal) || (role != Qt::DisplayRole)) {
        return QVariant();
    }
    
    switch (section) {
    case 0:
        return tr("Title");
    case 1:
        return tr("Artist");
    case 2:
        return tr("Genre");
    case 3:
        return tr("Duration");
    default:
        return QVariant();
    }
}

QVariant TrackModel::data(const QModelIndex &index, int role) const {
    if (const MKTrack *track = get(index.row())) {
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case 0:
                return track->title();
            case 1:
                return track->artist();
            case 2:
                return track->genre();
            case 3:
                return track->durationString();
            default:
                return QVariant();
            }
        }
        else if (role == Qt::FontRole) {
            if (index.row() == AudioPlayer::instance()->currentIndex()) {
                QFont font;
                font.setBold(true);
                return font;
            }
            
            return QVariant();
        }
        
        return track->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> TrackModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const MKTrack *track = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = track->property(iterator.value());
        }
    }
    
    return map;
}

QVariant TrackModel::data(int row, const QByteArray &role) const {
    if (const MKTrack *track = get(row)) {
        return track->property(role);
    }
    
    return QVariant();
}

QVariantMap TrackModel::itemData(int row) const {
    QVariantMap map;
    
    if (const MKTrack *track = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
            map[role] = track->property(role);
        }
    }
    
    return map;
}

MKTrack* TrackModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void TrackModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void TrackModel::append(MKTrack *track) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    track->setParent(this);
    connect(track, SIGNAL(changed()), this, SLOT(onItemChanged()));
    m_items << track;
    endInsertRows();
    emit countChanged(rowCount());
}

void TrackModel::insert(int row, MKTrack *track) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        track->setParent(this);
        connect(track, SIGNAL(changed()), this, SLOT(onItemChanged()));
        m_items.insert(row, track);
        endInsertRows();
        emit countChanged(rowCount());
    }
    else {
        append(track);
    }
}

void TrackModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
        emit countChanged(rowCount());
    }
}

void TrackModel::onCurrentIndexChanged() {
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void TrackModel::onItemChanged() {
    const int row = m_items.indexOf(qobject_cast<MKTrack*>(sender()));
    
    if (row != -1) {
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}
