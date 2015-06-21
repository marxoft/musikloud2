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

#include "soundcloudartist.h"
#include "soundcloud.h"
#include "resources.h"
#ifdef MUSIKLOUD_DEBUG
#include <QDebug>
#endif

SoundCloudArtist::SoundCloudArtist(QObject *parent) :
    MKArtist(parent),
    m_request(0),
    m_followed(false),
    m_followersCount(0),
    m_online(false),
    m_playlistCount(0),
    m_trackCount(0)
{
    setService(Resources::SOUNDCLOUD);
    connect(SoundCloud::instance(), SIGNAL(artistFollowed(SoundCloudArtist*)),
            this, SLOT(onArtistUpdated(SoundCloudArtist*)));
    connect(SoundCloud::instance(), SIGNAL(artistUnfollowed(SoundCloudArtist*)),
            this, SLOT(onArtistUpdated(SoundCloudArtist*)));
}

SoundCloudArtist::SoundCloudArtist(const QString &id, QObject *parent) :
    MKArtist(parent),
    m_request(0),
    m_followed(false),
    m_followersCount(0),
    m_online(false),
    m_playlistCount(0),
    m_trackCount(0)
{
    setService(Resources::SOUNDCLOUD);
    loadArtist(id);
    connect(SoundCloud::instance(), SIGNAL(artistFollowed(SoundCloudArtist*)),
            this, SLOT(onArtistUpdated(SoundCloudArtist*)));
    connect(SoundCloud::instance(), SIGNAL(artistUnfollowed(SoundCloudArtist*)),
            this, SLOT(onArtistUpdated(SoundCloudArtist*)));
}

SoundCloudArtist::SoundCloudArtist(const QVariantMap &artist, QObject *parent) :
    MKArtist(parent),
    m_request(0),
    m_followed(false),
    m_followersCount(0),
    m_online(false),
    m_playlistCount(0),
    m_trackCount(0)
{
    setService(Resources::SOUNDCLOUD);
    loadArtist(artist);
    connect(SoundCloud::instance(), SIGNAL(artistFollowed(SoundCloudArtist*)),
            this, SLOT(onArtistUpdated(SoundCloudArtist*)));
    connect(SoundCloud::instance(), SIGNAL(artistUnfollowed(SoundCloudArtist*)),
            this, SLOT(onArtistUpdated(SoundCloudArtist*)));
}

SoundCloudArtist::SoundCloudArtist(SoundCloudArtist *artist, QObject *parent) :
    MKArtist(artist, parent),
    m_request(0),
    m_followed(artist->isFollowed()),
    m_followersCount(artist->followersCount()),
    m_online(artist->isOnline()),
    m_playlistCount(artist->playlistCount()),
    m_trackCount(artist->trackCount()),
    m_websiteTitle(artist->websiteTitle()),
    m_websiteUrl(artist->websiteUrl())
{
    connect(SoundCloud::instance(), SIGNAL(artistFollowed(SoundCloudArtist*)),
            this, SLOT(onArtistUpdated(SoundCloudArtist*)));
    connect(SoundCloud::instance(), SIGNAL(artistUnfollowed(SoundCloudArtist*)),
            this, SLOT(onArtistUpdated(SoundCloudArtist*)));
}

QString SoundCloudArtist::errorString() const {
    return m_request ? SoundCloud::getErrorString(m_request->result().toMap()) : QString();
}

bool SoundCloudArtist::isFollowed() const {
    return m_followed;
}

void SoundCloudArtist::setFollowed(bool s) {
    if (s != isFollowed()) {
        m_followed = s;
        emit followedChanged();
    }
}

qint64 SoundCloudArtist::followersCount() const {
    return m_followersCount;
}

void SoundCloudArtist::setFollowersCount(qint64 c) {
    if (c != followersCount()) {
        m_followersCount = c;
        emit followersCountChanged();
    }
}

bool SoundCloudArtist::isOnline() const {
    return m_online;
}

void SoundCloudArtist::setOnline(bool o) {
    if (o != isOnline()) {
        m_online = o;
        emit onlineChanged();
    }
}

qint64 SoundCloudArtist::playlistCount() const {
    return m_playlistCount;
}

void SoundCloudArtist::setPlaylistCount(qint64 c) {
    if (c != playlistCount()) {
        m_playlistCount = c;
        emit playlistCountChanged();
    }
}

QSoundCloud::ResourcesRequest::Status SoundCloudArtist::status() const {
    return m_request ? m_request->status() : QSoundCloud::ResourcesRequest::Null;
}

qint64 SoundCloudArtist::trackCount() const {
    return m_trackCount;
}

void SoundCloudArtist::setTrackCount(qint64 c) {
    if (c != trackCount()) {
        m_trackCount = c;
        emit trackCountChanged();
    }
}

QString SoundCloudArtist::websiteTitle() const {
    return m_websiteTitle;
}

void SoundCloudArtist::setWebsiteTitle(const QString &t) {
    if (t != websiteTitle()) {
        m_websiteTitle = t;
        emit websiteTitleChanged();
    }
}

QUrl SoundCloudArtist::websiteUrl() const {
    return m_websiteUrl;
}

void SoundCloudArtist::setWebsiteUrl(const QUrl &u) {
    if (u != websiteUrl()) {
        m_websiteUrl = u;
        emit websiteUrlChanged();
    }
}

void SoundCloudArtist::loadArtist(const QString &id) {
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
        m_request->get(id.isEmpty() ? "/me" : "/artists/" + id);
    }
    
    connect(m_request, SIGNAL(finished()), this, SLOT(onArtistRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudArtist::loadArtist(const QVariantMap &artist) {
    setDescription(artist.value("description").toString());
    setFollowersCount(artist.value("followers_count").toLongLong());
    setId(artist.value("id").toString());
    setLargeThumbnailUrl(artist.value("avatar_url").toString());
    setThumbnailUrl(artist.value("avatar_url").toString());
    setName(artist.value("username").toString());
    setOnline(artist.value("online").toBool());
    setPlaylistCount(artist.value("playlist_count").toLongLong());
    setTrackCount(artist.value("track_count").toLongLong());
    setWebsiteTitle(artist.value("website_title").toString());
    setWebsiteUrl(artist.value("website_url").toString());
}

void SoundCloudArtist::loadArtist(SoundCloudArtist *artist) {
    MKArtist::loadArtist(artist);
    setFollowed(artist->isFollowed());
    setFollowersCount(artist->followersCount());
    setOnline(artist->isOnline());
    setPlaylistCount(artist->playlistCount());
    setTrackCount(artist->trackCount());
    setWebsiteTitle(artist->websiteTitle());
    setWebsiteUrl(artist->websiteUrl());
}

void SoundCloudArtist::checkIfFollowed() {
    if (SoundCloud::followingCache.ids.contains(id())) {
        setFollowed(true);
        return;
    }
    
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    if ((SoundCloud::followingCache.loaded) && (SoundCloud::followingCache.nextHref.isEmpty())) {
#ifdef MUSIKLOUD_DEBUG
        qDebug() << "SoundCloudArtist::checkIfFollowed" << id() << "not found";
#endif
        setFollowed(false);
        return;
    }
    
    initRequest();
    
    if (SoundCloud::followingCache.nextHref.isEmpty()) {
        m_request->get("/me/followings", SoundCloud::followingCache.filters);
    }
    else {
        m_request->get(SoundCloud::followingCache.nextHref);
    }
    
    connect(m_request, SIGNAL(finished()), this, SLOT(onFollowCheckRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudArtist::follow() {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->insert("/me/followings/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onFollowRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudArtist::unfollow() {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->del("/me/followings/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onUnfollowRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudArtist::initRequest() {
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

void SoundCloudArtist::onArtistRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        loadArtist(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onArtistRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudArtist::onFollowCheckRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        QVariantList list = result.value("collection").toList();
        SoundCloud::followingCache.nextHref = result.value("next_href").toString().section('/', -1);
        SoundCloud::followingCache.loaded = true;

        foreach (QVariant item, list) {
            QVariantMap map = item.toMap();
            SoundCloud::followingCache.ids.append(map.value("id").toString());
        }
#ifdef MUSIKLOUD_DEBUG
        qDebug() << "SoundCloudArtist::onFollowCheckRequestFinished OK" << SoundCloud::followingCache.ids;
#endif
        disconnect(m_request, SIGNAL(finished()), this, SLOT(onArtistRequestFinished()));
        emit statusChanged(status());
        checkIfFollowed();
        return;
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onArtistRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudArtist::onFollowRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        setFollowed(true);
        setFollowersCount(followersCount() + 1);
        SoundCloud::followingCache.ids.append(id());
        emit SoundCloud::instance()->artistFollowed(this);
#ifdef MUSIKLOUD_DEBUG
        qDebug() << "SoundCloudArtist::onFollowRequestFinished OK" << id();
#endif
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onFollowRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudArtist::onUnfollowRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        setFollowed(false);
        setFollowersCount(followersCount() - 1);
        SoundCloud::followingCache.ids.removeOne(id());
        emit SoundCloud::instance()->artistUnfollowed(this);
#ifdef MUSIKLOUD_DEBUG
        qDebug() << "SoundCloudArtist::onUnfollowRequestFinished OK" << id();
#endif
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUnfollowRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudArtist::onArtistUpdated(SoundCloudArtist *artist) {
    if ((artist->id() == id()) && (artist != this)) {
        loadArtist(artist);
    }
}
