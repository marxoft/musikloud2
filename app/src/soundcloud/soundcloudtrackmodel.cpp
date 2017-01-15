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

#include "soundcloudtrackmodel.h"
#include "logger.h"
#include "soundcloud.h"
#include <qsoundcloud/urls.h>

SoundCloudTrackModel::SoundCloudTrackModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QSoundCloud::ResourcesRequest(this))
{
    m_roles[ArtistRole] = "artist";
    m_roles[ArtistIdRole] = "artistId";
    m_roles[CommentableRole] = "commentable";
    m_roles[DateRole] = "date";
    m_roles[DescriptionRole] = "description";
    m_roles[DownloadableRole] = "downloadable";
    m_roles[DurationRole] = "duration";
    m_roles[DurationStringRole] = "durationString";
    m_roles[ErrorStringRole] = "errorString";
    m_roles[FavouriteRole] = "favourited";
    m_roles[FavouriteCountRole] = "favouriteCount";
    m_roles[GenreRole] = "genre";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[PlayCountRole] = "playCount";
    m_roles[SharingRole] = "sharing";
    m_roles[SizeRole] = "size";
    m_roles[SizeStringRole] = "sizeString";
    m_roles[StatusRole] = "status";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
    m_roles[UrlRole] = "url";
    m_roles[WaveformUrlRole] = "waveformUrl";
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

QString SoundCloudTrackModel::errorString() const {
    return SoundCloud::getErrorString(m_request->result().toMap());
}

QSoundCloud::ResourcesRequest::Status SoundCloudTrackModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> SoundCloudTrackModel::roleNames() const {
    return m_roles;
}
#endif

int SoundCloudTrackModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_items.size();
}

int SoundCloudTrackModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 4;
}

bool SoundCloudTrackModel::canFetchMore(const QModelIndex &parent) const {
    return (!parent.isValid()) && (status() != QSoundCloud::ResourcesRequest::Loading) && (!m_nextHref.isEmpty());
}

void SoundCloudTrackModel::fetchMore(const QModelIndex &parent) {
    if (!canFetchMore(parent)) {
        return;
    }
    
    m_request->get(m_nextHref);
    emit statusChanged(status());
}

QVariant SoundCloudTrackModel::headerData(int section, Qt::Orientation orientation, int role) const {
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

QVariant SoundCloudTrackModel::data(const QModelIndex &index, int role) const {
    if (const SoundCloudTrack *track = get(index.row())) {
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
        
        return track->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> SoundCloudTrackModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const SoundCloudTrack *track = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = track->property(iterator.value());
        }
    }
    
    return map;
}

QVariant SoundCloudTrackModel::data(int row, const QByteArray &role) const {
    if (const SoundCloudTrack *track = get(row)) {
        return track->property(role);
    }
    
    return QVariant();
}

QVariantMap SoundCloudTrackModel::itemData(int row) const {
    QVariantMap map;
    
    if (const SoundCloudTrack *track = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
            map[role] = track->property(role);
        }
    }
    
    return map;
}

SoundCloudTrack* SoundCloudTrackModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void SoundCloudTrackModel::get(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("SoundCloudTrackModel::get(). Resource path: " + resourcePath, Logger::HighVerbosity);
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    
    if (!resourcePath.contains("/playlists/")) {
        m_filters["linked_partitioning"] = true;
    }
    
    m_request->get(m_resourcePath, m_filters);
    emit statusChanged(status());
    
    disconnect(SoundCloud::instance(), 0, this, 0);
        
    if (resourcePath == "/me/favorites") {
        connect(SoundCloud::instance(), SIGNAL(trackFavourited(SoundCloudTrack*)),
                this, SLOT(onTrackFavourited(SoundCloudTrack*)));
        connect(SoundCloud::instance(), SIGNAL(trackUnfavourited(SoundCloudTrack*)),
                this, SLOT(onTrackUnfavourited(SoundCloudTrack*)));
    }
}

void SoundCloudTrackModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_nextHref = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void SoundCloudTrackModel::cancel() {
    m_request->cancel();
}

void SoundCloudTrackModel::reload() {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("SoundCloudTrackModel::reload(). Resource path: " + m_resourcePath, Logger::HighVerbosity);
    clear();
    m_request->get(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void SoundCloudTrackModel::append(SoundCloudTrack *track) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    connect(track, SIGNAL(changed()), this, SLOT(onItemChanged()));
    m_items << track;
    endInsertRows();
}

void SoundCloudTrackModel::insert(int row, SoundCloudTrack *track) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        connect(track, SIGNAL(changed()), this, SLOT(onItemChanged()));
        m_items.insert(row, track);
        endInsertRows();
    }
    else {
        append(track);
    }
}

void SoundCloudTrackModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void SoundCloudTrackModel::onItemChanged() {
    const int row = m_items.indexOf(qobject_cast<SoundCloudTrack*>(sender()));
    
    if (row != -1) {
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

void SoundCloudTrackModel::onRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_nextHref = result.value("next_href").toString().section(QSoundCloud::API_URL, -1);
            const QVariantList list = result.value(m_resourcePath.contains("/playlists/") ? "tracks" : "collection").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (const QVariant &item, list) {
                SoundCloudTrack *track = new SoundCloudTrack(item.toMap(), this);
                connect(track, SIGNAL(changed()), this, SLOT(onItemChanged()));
                m_items << track;
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    else {
        Logger::log("SoundCloudTrackModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}

void SoundCloudTrackModel::onTrackFavourited(SoundCloudTrack *track) {
    insert(0, new SoundCloudTrack(track, this));
}

void SoundCloudTrackModel::onTrackUnfavourited(SoundCloudTrack *track) {
    const QModelIndexList list = match(index(0), IdRole, track->id(), 1, Qt::MatchExactly);
    
    if (!list.isEmpty()) {
        remove(list.first().row());
    }
}
