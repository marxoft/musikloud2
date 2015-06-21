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

#include "soundcloudplaylist.h"
#include "soundcloud.h"
#include "resources.h"
#include <QDateTime>

SoundCloudPlaylist::SoundCloudPlaylist(QObject *parent) :
    MKPlaylist(parent),
    m_request(0)
{
    setService(Resources::SOUNDCLOUD);
}

SoundCloudPlaylist::SoundCloudPlaylist(const QString &id, QObject *parent) :
    MKPlaylist(parent),
    m_request(0)
{
    setService(Resources::SOUNDCLOUD);
    loadPlaylist(id);
}

SoundCloudPlaylist::SoundCloudPlaylist(const QVariantMap &playlist, QObject *parent) :
    MKPlaylist(parent),
    m_request(0)
{
    setService(Resources::SOUNDCLOUD);
    loadPlaylist(playlist);
}

SoundCloudPlaylist::SoundCloudPlaylist(SoundCloudPlaylist *playlist, QObject *parent) :
    MKPlaylist(playlist, parent),
    m_request(0),
    m_sharing(playlist->sharing())
{
}

QString SoundCloudPlaylist::errorString() const {
    return m_request ? SoundCloud::getErrorString(m_request->result().toMap()) : QString();
}

QString SoundCloudPlaylist::sharing() const {
    return m_sharing;
}

void SoundCloudPlaylist::setSharing(const QString &s) {
    if (s != sharing()) {
        m_sharing = s;
        emit sharingChanged();
    }
}

QSoundCloud::ResourcesRequest::Status SoundCloudPlaylist::status() const {
    return m_request ? m_request->status() : QSoundCloud::ResourcesRequest::Null;
}

void SoundCloudPlaylist::loadPlaylist(const QString &id) {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    
    if (id.startsWith("http")) {
        QVariantMap filters;
        filters["url"] = id;
        m_request->get("/resolve", filters);
    }
    else {
        m_request->get("/playlists/" + id);
    }
    
    connect(m_request, SIGNAL(finished()), this, SLOT(onPlaylistRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudPlaylist::loadPlaylist(const QVariantMap &playlist) {
    QVariantMap user = playlist.value("user").toMap();
    
    setArtist(user.value("username").toString());
    setArtistId(user.value("id").toString());
    setDate(QDateTime::fromString(playlist.value("created_at").toString(),
                                  "yyyy/MM/dd HH:mm:ss +0000").toString("dd MMM yyyy"));
    setDescription(playlist.value("description").toString());
    setDuration(playlist.value("duration").toLongLong());
    setGenre(playlist.value("genre").toString());
    setId(playlist.value("id").toString());
    setLargeThumbnailUrl(playlist.value("artwork_url").toString());
    setSharing(playlist.value("sharing").toString());
    setThumbnailUrl(playlist.value("artwork_url").toString());
    setTitle(playlist.value("title").toString());
    setTrackCount(playlist.value("track_count").toInt());    
}

void SoundCloudPlaylist::loadPlaylist(SoundCloudPlaylist *playlist) {
    MKPlaylist::loadPlaylist(playlist);
    setSharing(playlist->sharing());
}

void SoundCloudPlaylist::initRequest() {
    if (!m_request) {
        m_request = new QSoundCloud::ResourcesRequest(this);
        m_request->setClientId(SoundCloud::instance()->clientId());
        m_request->setClientSecret(SoundCloud::instance()->clientSecret());
        m_request->setAccessToken(SoundCloud::instance()->accessToken());
        m_request->setRefreshToken(SoundCloud::instance()->refreshToken());
    
        connect(m_request, SIGNAL(accessTokenChanged(QString)), SoundCloud::instance(), SLOT(setAccessToken(QString)));
        connect(m_request, SIGNAL(refreshTokenChanged(QString)), SoundCloud::instance(), SLOT(setRefreshToken(QString)));
    }
}

void SoundCloudPlaylist::onPlaylistRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        loadPlaylist(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onPlaylistRequestFinished()));
    emit statusChanged(status());
}
