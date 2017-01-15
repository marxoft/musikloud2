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
#include "logger.h"
#include "soundcloud.h"
#include <qsoundcloud/urls.h>

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
    m_roles[ErrorStringRole] = "errorString";
    m_roles[GenreRole] = "genre";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[SharingRole] = "sharing";
    m_roles[StatusRole] = "status";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
    m_roles[TrackCountRole] = "trackCount";
    m_roles[UrlRole] = "url";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    m_request->setClientId(SoundCloud::clientId());
    m_request->setClientSecret(SoundCloud::clientSecret());
    m_request->setAccessToken(SoundCloud::accessToken());
    m_request->setRefreshToken(SoundCloud::refreshToken());
        
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

int SoundCloudPlaylistModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_items.size();
}

int SoundCloudPlaylistModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 4;
}

bool SoundCloudPlaylistModel::canFetchMore(const QModelIndex &parent) const {
    return (!parent.isValid()) && (status() != QSoundCloud::ResourcesRequest::Loading) && (!m_nextHref.isEmpty());
}

void SoundCloudPlaylistModel::fetchMore(const QModelIndex &parent) {
    if (!canFetchMore(parent)) {
        return;
    }
    
    m_request->get(m_nextHref);
    emit statusChanged(status());
}

QVariant SoundCloudPlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((orientation != Qt::Horizontal) || (role != Qt::DisplayRole)) {
        return QVariant();
    }
    
    switch (section) {
    case 0:
        return tr("Title");
    case 1:
        return tr("Artist");
    case 2:
        return tr("Tracks");
    case 3:
        return tr("Duration");
    default:
        return QVariant();
    }
}

QVariant SoundCloudPlaylistModel::data(const QModelIndex &index, int role) const {
    if (const SoundCloudPlaylist *playlist = get(index.row())) {
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case 0:
                return playlist->title();
            case 1:
                return playlist->artist();
            case 2:
                return playlist->trackCount();
            case 3:
                return playlist->durationString();
            default:
                return QVariant();
            }
        }
        
        return playlist->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> SoundCloudPlaylistModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const SoundCloudPlaylist *playlist = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = playlist->property(iterator.value());
        }
    }
    
    return map;
}

QVariant SoundCloudPlaylistModel::data(int row, const QByteArray &role) const {
    if (const SoundCloudPlaylist *playlist = get(row)) {
        return playlist->property(role);
    }
    
    return QVariant();
}

QVariantMap SoundCloudPlaylistModel::itemData(int row) const {
    QVariantMap map;
    
    if (const SoundCloudPlaylist *playlist = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
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
    
    Logger::log("SoundCloudPlaylistModel::get(). Resource path: " + resourcePath, Logger::HighVerbosity);
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
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("SoundCloudPlaylistModel::reload(). Resource path: " + m_resourcePath, Logger::HighVerbosity);
    clear();
    m_request->get(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void SoundCloudPlaylistModel::append(SoundCloudPlaylist *playlist) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    connect(playlist, SIGNAL(changed()), this, SLOT(onItemChanged()));
    m_items << playlist;
    endInsertRows();
}

void SoundCloudPlaylistModel::insert(int row, SoundCloudPlaylist *playlist) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        connect(playlist, SIGNAL(changed()), this, SLOT(onItemChanged()));
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

void SoundCloudPlaylistModel::onItemChanged() {
    const int row = m_items.indexOf(qobject_cast<SoundCloudPlaylist*>(sender()));
    
    if (row != -1) {
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

void SoundCloudPlaylistModel::onRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_nextHref = result.value("next_href").toString().section(QSoundCloud::API_URL, -1);
            const QVariantList list = result.value("collection").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (const QVariant &item, list) {
                SoundCloudPlaylist *playlist = new SoundCloudPlaylist(item.toMap(), this);
                connect(playlist, SIGNAL(changed()), this, SLOT(onItemChanged()));
                m_items << playlist;
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    else {
        Logger::log("SoundCloudPlaylistModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}
