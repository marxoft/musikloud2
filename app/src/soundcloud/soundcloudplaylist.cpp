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
#include "definitions.h"
#include "resources.h"
#include "soundcloud.h"
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

SoundCloudPlaylist::SoundCloudPlaylist(const SoundCloudPlaylist *playlist, QObject *parent) :
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
        emit changed();
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
    
    setId(id);
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
    emit changed();
    emit statusChanged(status());
}

void SoundCloudPlaylist::loadPlaylist(const QVariantMap &playlist) {
    const QVariantMap user = playlist.value("user").toMap();
    const QString thumbnail = playlist.value("artwork_url").toString();
    
    setArtist(user.value("username").toString());
    setArtistId(user.value("id").toString());
    setDate(QDateTime::fromString(playlist.value("created_at").toString(),
                                  "yyyy/MM/dd HH:mm:ss +0000").toString("dd MMM yyyy"));
    setDescription(playlist.value("description").toString());
    setDuration(playlist.value("duration").toLongLong());
    setGenre(playlist.value("genre").toString());
    setId(playlist.value("id").toString());
    setSharing(playlist.value("sharing").toString());
    setTitle(playlist.value("title").toString());
    setTrackCount(playlist.value("track_count").toInt());
    setUrl(playlist.value("url").toString());

    if (!thumbnail.isEmpty()) {
        setLargeThumbnailUrl(QString("%1-t%2x%2.jpg").arg(thumbnail.left(thumbnail.lastIndexOf('-'))).arg(LARGE_THUMBNAIL_SIZE));
        setThumbnailUrl(QString("%1-t%2x%2.jpg").arg(thumbnail.left(thumbnail.lastIndexOf('-'))).arg(THUMBNAIL_SIZE));
    }
    else {
        setLargeThumbnailUrl(QString());
        setThumbnailUrl(QString());
    }
}

void SoundCloudPlaylist::loadPlaylist(SoundCloudPlaylist *playlist) {
    MKPlaylist::loadPlaylist(playlist);
    setSharing(playlist->sharing());
}

void SoundCloudPlaylist::cancel() {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        m_request->cancel();
        emit changed();
        emit statusChanged(status());
    }
}

void SoundCloudPlaylist::initRequest() {
    if (!m_request) {
        m_request = new QSoundCloud::ResourcesRequest(this);
        m_request->setClientId(SoundCloud::clientId());
        m_request->setClientSecret(SoundCloud::clientSecret());
        m_request->setAccessToken(SoundCloud::accessToken());
        m_request->setRefreshToken(SoundCloud::refreshToken());
    
        connect(m_request, SIGNAL(accessTokenChanged(QString)), SoundCloud::instance(), SLOT(setAccessToken(QString)));
        connect(m_request, SIGNAL(refreshTokenChanged(QString)), SoundCloud::instance(), SLOT(setRefreshToken(QString)));
    }
}

void SoundCloudPlaylist::onPlaylistRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        loadPlaylist(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onPlaylistRequestFinished()));
    emit changed();
    emit statusChanged(status());
}
