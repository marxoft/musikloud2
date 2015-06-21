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

#include "soundcloudartistmodel.h"
#include "soundcloud.h"
#ifdef MUSIKLOUD_DEBUG
#include <QDebug>
#endif

SoundCloudArtistModel::SoundCloudArtistModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QSoundCloud::ResourcesRequest(this))
{
    m_roles[DescriptionRole] = "description";
    m_roles[FollowedRole] = "followed";
    m_roles[FollowersCountRole] = "followersCount";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[NameRole] = "name";
    m_roles[OnlineRole] = "online";
    m_roles[PlaylistCountRole] = "playlistCount";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TrackCountRole] = "trackCount";
    m_roles[WebsiteTitleRole] = "websiteTitle";
    m_roles[WebsiteUrlRole] = "websiteUrl";
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

QString SoundCloudArtistModel::errorString() const {
    return SoundCloud::getErrorString(m_request->result().toMap());
}

QSoundCloud::ResourcesRequest::Status SoundCloudArtistModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> SoundCloudArtistModel::roleNames() const {
    return m_roles;
}
#endif

int SoundCloudArtistModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool SoundCloudArtistModel::canFetchMore(const QModelIndex &) const {
    return (status() != QSoundCloud::ResourcesRequest::Loading) && (!m_nextHref.isEmpty());
}

void SoundCloudArtistModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    m_request->get(m_nextHref);
    emit statusChanged(status());
}

QVariant SoundCloudArtistModel::data(const QModelIndex &index, int role) const {
    if (SoundCloudArtist *artist = get(index.row())) {
        return artist->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> SoundCloudArtistModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (SoundCloudArtist *artist = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = artist->property(iterator.value());
        }
    }
    
    return map;
}

QVariant SoundCloudArtistModel::data(int row, const QByteArray &role) const {
    if (SoundCloudArtist *artist = get(row)) {
        return artist->property(role);
    }
    
    return QVariant();
}

QVariantMap SoundCloudArtistModel::itemData(int row) const {
    QVariantMap map;
    
    if (SoundCloudArtist *artist = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = artist->property(role);
        }
    }
    
    return map;
}

SoundCloudArtist* SoundCloudArtistModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void SoundCloudArtistModel::get(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_filters["linked_partitioning"] = true;
    m_request->get(m_resourcePath, m_filters);
    emit statusChanged(status());
    
    disconnect(SoundCloud::instance(), 0, this, 0);
    
    if (resourcePath == "/me/followings") {
        connect(SoundCloud::instance(), SIGNAL(artistFollowed(SoundCloudArtist*)),
                this, SLOT(onArtistFollowed(SoundCloudArtist*)));
        connect(SoundCloud::instance(), SIGNAL(artistUnfollowed(SoundCloudArtist*)),
                this, SLOT(onArtistUnfollowed(SoundCloudArtist*)));
    }
}

void SoundCloudArtistModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_nextHref = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void SoundCloudArtistModel::cancel() {
    m_request->cancel();
}

void SoundCloudArtistModel::reload() {
    clear();
    m_request->get(m_resourcePath,  m_filters);
    emit statusChanged(status());
}

void SoundCloudArtistModel::append(SoundCloudArtist *artist) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << artist;
    endInsertRows();
}

void SoundCloudArtistModel::insert(int row, SoundCloudArtist *artist) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, artist);
        endInsertRows();
    }
    else {
        append(artist);
    }
}

void SoundCloudArtistModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void SoundCloudArtistModel::onRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_nextHref = result.value("next_href").toString().section('/', -1);
            QVariantList list = result.value("collection").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new SoundCloudArtist(item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    
    emit statusChanged(status());
}

void SoundCloudArtistModel::onArtistFollowed(SoundCloudArtist *artist) {
    insert(0, new SoundCloudArtist(artist, this));
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "SoundCloudArtistModel::onArtistFollowed" << artist->id();
#endif
}

void SoundCloudArtistModel::onArtistUnfollowed(SoundCloudArtist *artist) {
    QModelIndexList list = match(index(0), IdRole, artist->id(), 1, Qt::MatchExactly);
    
    if (!list.isEmpty()) {
        remove(list.first().row());
    }
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "SoundCloudArtistModel::onArtistUnfollowed" << artist->id();
#endif
}
