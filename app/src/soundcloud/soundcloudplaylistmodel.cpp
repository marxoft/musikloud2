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

#include "soundcloudplaylistmodel.h"
#include "soundcloud.h"

SoundCloudPlaylistModel::SoundCloudPlaylistModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QSoundCloud::ResourcesRequest(this))
{
    m_roles[ArtistRole] = "artist";
    m_roles[ArtistIdRole] = "artistId";
    m_roles[DateRole] = "date";
    m_roles[DescriptionRole] = "description";
    m_roles[DurationRole] = "duration";
    m_roles[DurationStringRole] = "durationString";
    m_roles[GenreRole] = "genre";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[SharingRole] = "sharing";
    m_roles[TitleRole] = "title";
    m_roles[TrackCountRole] = "trackCount";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    m_request->setClientId(SoundCloud::instance()->clientId());
    m_request->setClientSecret(SoundCloud::instance()->clientSecret());
    m_request->setAccessToken(SoundCloud::instance()->accessToken());
    m_request->setRefreshToken(SoundCloud::instance()->refreshToken());
        
    connect(m_request, SIGNAL(accessTokenChanged(QString)), SoundCloud::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(refreshTokenChanged(QString)), SoundCloud::instance(), SLOT(setRefreshToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString SoundCloudPlaylistModel::errorString() const {
    return SoundCloud::getErrorString(m_request->result().toMap());
}

QSoundCloud::ResourcesRequest::Status SoundCloudPlaylistModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> SoundCloudPlaylistModel::roleNames() const {
    return m_roles;
}
#endif

int SoundCloudPlaylistModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool SoundCloudPlaylistModel::canFetchMore(const QModelIndex &) const {
    return (status() != QSoundCloud::ResourcesRequest::Loading) && (!m_nextHref.isEmpty());
}

void SoundCloudPlaylistModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    m_request->get(m_nextHref);
    emit statusChanged(status());
}

QVariant SoundCloudPlaylistModel::data(const QModelIndex &index, int role) const {
    if (SoundCloudPlaylist *playlist = get(index.row())) {
        return playlist->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> SoundCloudPlaylistModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (SoundCloudPlaylist *playlist = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = playlist->property(iterator.value());
        }
    }
    
    return map;
}

QVariant SoundCloudPlaylistModel::data(int row, const QByteArray &role) const {
    if (SoundCloudPlaylist *playlist = get(row)) {
        return playlist->property(role);
    }
    
    return QVariant();
}

QVariantMap SoundCloudPlaylistModel::itemData(int row) const {
    QVariantMap map;
    
    if (SoundCloudPlaylist *playlist = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = playlist->property(role);
        }
    }
    
    return map;
}

SoundCloudPlaylist* SoundCloudPlaylistModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void SoundCloudPlaylistModel::get(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_filters["representation"] = "compact";
    m_filters["linked_partitioning"] = true;
    m_request->get(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void SoundCloudPlaylistModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_nextHref = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void SoundCloudPlaylistModel::cancel() {
    m_request->cancel();
}

void SoundCloudPlaylistModel::reload() {
    clear();
    m_request->get(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void SoundCloudPlaylistModel::append(SoundCloudPlaylist *playlist) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << playlist;
    endInsertRows();
}

void SoundCloudPlaylistModel::insert(int row, SoundCloudPlaylist *playlist) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, playlist);
        endInsertRows();
    }
    else {
        append(playlist);
    }
}

void SoundCloudPlaylistModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void SoundCloudPlaylistModel::onRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_nextHref = result.value("next_href").toString().section('/', -1);
            QVariantList list = result.value("collection").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new SoundCloudPlaylist(item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    
    emit statusChanged(status());
}
