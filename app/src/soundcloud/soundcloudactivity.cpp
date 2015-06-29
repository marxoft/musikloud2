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

#include "soundcloudactivity.h"
#include "soundcloud.h"
#include "utils.h"
#include <QDateTime>

SoundCloudActivity::SoundCloudActivity(QObject *parent) :
    QObject(parent),
    m_request(0)
{
}

SoundCloudActivity::SoundCloudActivity(const QString &id, QObject *parent) :
    QObject(parent),
    m_request(0)
{
    loadActivity(id);
}

SoundCloudActivity::SoundCloudActivity(const QVariantMap &activity, QObject *parent) :
    QObject(parent),
    m_request(0)
{
    loadActivity(activity);
}

SoundCloudActivity::SoundCloudActivity(SoundCloudActivity *activity, QObject *parent) :
    QObject(parent),
    m_request(0),
    m_type(activity->activityType()),
    m_typeString(activity->activityTypeString()),
    m_artist(activity->artist()),
    m_artistId(activity->artistId()),
    m_artistThumbnailUrl(activity->artistThumbnailUrl()),
    m_date(activity->date()),
    m_description(activity->description()),
    m_id(activity->id()),
    m_originId(activity->originId()),
    m_originThumbnailUrl(activity->originThumbnailUrl()),
    m_title(activity->title())
{
}

QString SoundCloudActivity::activityType() const {
    return m_type;
}

void SoundCloudActivity::setActivityType(const QString &t) {
    if (t != activityType()) {
        m_type = t;
        
        if (t == "track") {
            m_typeString = tr("New track");
        }
        else if (t == "playlist") {
            m_typeString = tr("New playlist");
        }
        else if (t == "track-repost") {
            m_typeString = tr("Track repost");
        }
        else if (t == "favoriting") {
            m_typeString = tr("Track favouriting");
        }
        else if (t == "track-sharing") {
            m_typeString = tr("Track sharing");
        }
        else if (t == "comment") {
            m_typeString = tr("Comment");
        }
        else {
            m_typeString = tr("Unknown");
        }
        
        emit activityTypeChanged();
    }
}

QString SoundCloudActivity::activityTypeString() const {
    return m_typeString;
}

QString SoundCloudActivity::artist() const {
    return m_artist;
}

void SoundCloudActivity::setArtist(const QString &a) {
    if (a != artist()) {
        m_artist = a;
        emit artistChanged();
    }
}

QString SoundCloudActivity::artistId() const {
    return m_artistId;
}

void SoundCloudActivity::setArtistId(const QString &i) {
    if (i != artistId()) {
        m_artistId = i;
        emit artistIdChanged();
    }
}

QUrl SoundCloudActivity::artistThumbnailUrl() const {
    return m_artistThumbnailUrl;
}

void SoundCloudActivity::setArtistThumbnailUrl(const QUrl &u) {
    if (u != artistThumbnailUrl()) {
        m_artistThumbnailUrl = u;
        emit artistThumbnailUrlChanged();
    }
}

QString SoundCloudActivity::date() const {
    return m_date;
}

void SoundCloudActivity::setDate(const QString &d) {
    if (d != date()) {
        m_date = d;
        emit dateChanged();
    }
}

QString SoundCloudActivity::description() const {
    return m_description;
}

void SoundCloudActivity::setDescription(const QString &d) {
    if (d != description()) {
        m_description = d;
        emit descriptionChanged();
    }
}

QString SoundCloudActivity::errorString() const {
    return m_request ? SoundCloud::getErrorString(m_request->result().toMap()) : QString();
}

QString SoundCloudActivity::id() const {
    return m_id;
}

void SoundCloudActivity::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit idChanged();
    }
}

QString SoundCloudActivity::originId() const {
    return m_originId;
}

void SoundCloudActivity::setOriginId(const QString &i) {
    if (i != originId()) {
        m_originId = i;
        emit originIdChanged();
    }
}

QUrl SoundCloudActivity::originThumbnailUrl() const {
    return m_originThumbnailUrl;
}

void SoundCloudActivity::setOriginThumbnailUrl(const QUrl &u) {
    if (u != originThumbnailUrl()) {
        m_originThumbnailUrl = u;
        emit originThumbnailUrlChanged();
    }
}

QSoundCloud::ResourcesRequest::Status SoundCloudActivity::status() const {
    return m_request ? m_request->status() : QSoundCloud::ResourcesRequest::Null;
}

QString SoundCloudActivity::title() const {
    return m_title;
}

void SoundCloudActivity::setTitle(const QString &t) {
    if (t != title()) {
        m_title = t;
        emit titleChanged();
    }
}

void SoundCloudActivity::loadActivity(const QString &id) {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->get("/me/activities/" + id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onActivityRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudActivity::loadActivity(const QVariantMap &activity) {
    setActivityType(activity.value("type").toString());
    
    if (activityType() == "comment") {
        loadCommentActivity(activity);
    }
    else if (activityType() == "playlist") {
        loadPlaylistActivity(activity);
    }
    else if (activityType() == "track") {
        loadTrackActivity(activity);
    }
    else if (activityType() == "favoriting") {
        loadTrackFavouritingActivity(activity);
    }
    else if (activityType() == "track-repost") {
        loadTrackRepostActivity(activity);
    }
    else {
        loadTrackSharingActivity(activity);
    }
}

void SoundCloudActivity::loadCommentActivity(const QVariantMap &activity) {
    QVariantMap origin = activity.value("origin").toMap();
    QVariantMap track = origin.value("track").toMap();
    QVariantMap user = origin.value("user").toMap();
    
    setArtist(user.value("username").toString());
    setArtistId(user.value("id").toString());
    setArtistThumbnailUrl(user.value("avatar_url").toString());
    setDate(QDateTime::fromString(activity.value("created_at").toString(),
                                  "yyyy/MM/dd HH:mm:ss +0000").toString("dd MMM yyyy"));
    setDescription(origin.value("body").toString());
    setId(activity.value("id").toString());
    setOriginId(track.value("id").toString());
    setOriginThumbnailUrl(track.value("artwork_url").toString());
    setTitle(track.value("title").toString());
}

void SoundCloudActivity::loadPlaylistActivity(const QVariantMap &activity) {
    QVariantMap playlist = activity.value("origin").toMap();
    QVariantMap user = playlist.value("user").toMap();
    
    setArtist(user.value("username").toString());
    setArtistId(user.value("id").toString());
    setArtistThumbnailUrl(user.value("avatar_url").toString());
    setDate(QDateTime::fromString(activity.value("created_at").toString(),
                                  "yyyy/MM/dd HH:mm:ss +0000").toString("dd MMM yyyy"));
    setDescription(playlist.value("description").toString());
    setId(activity.value("id").toString());
    setOriginId(playlist.value("id").toString());
    setOriginThumbnailUrl(playlist.value("artwork_url").toString());
    setTitle(playlist.value("title").toString());
}

void SoundCloudActivity::loadTrackActivity(const QVariantMap &activity) {
    QVariantMap track = activity.value("origin").toMap();
    QVariantMap user = track.value("user").toMap();
    
    setArtist(user.value("username").toString());
    setArtistId(user.value("id").toString());
    setArtistThumbnailUrl(user.value("avatar_url").toString());
    setDate(QDateTime::fromString(activity.value("created_at").toString(),
                                  "yyyy/MM/dd HH:mm:ss +0000").toString("dd MMM yyyy"));
    setDescription(track.value("description").toString());
    setId(activity.value("id").toString());
    setOriginId(track.value("id").toString());
    setOriginThumbnailUrl(track.value("artwork_url").toString());
    setTitle(track.value("title").toString());
}

void SoundCloudActivity::loadTrackFavouritingActivity(const QVariantMap &activity) {
    QVariantMap origin = activity.value("origin").toMap();
    QVariantMap track = origin.value("track").toMap();
    QVariantMap user = origin.value("user").toMap();
    
    setArtist(user.value("username").toString());
    setArtistId(user.value("id").toString());
    setArtistThumbnailUrl(user.value("avatar_url").toString());
    setDate(QDateTime::fromString(activity.value("created_at").toString(),
                                  "yyyy/MM/dd HH:mm:ss +0000").toString("dd MMM yyyy"));
    setDescription(QString("%1 %2").arg(tr("Favourited by")).arg(artist()));
    setId(activity.value("id").toString());
    setOriginId(track.value("id").toString());
    setOriginThumbnailUrl(track.value("artwork_url").toString());
    setTitle(track.value("title").toString());
}

void SoundCloudActivity::loadTrackRepostActivity(const QVariantMap &activity) {
    QVariantMap track = activity.value("origin").toMap();
    QVariantMap user = track.value("user").toMap();
    
    setArtist(user.value("username").toString());
    setArtistId(user.value("id").toString());
    setArtistThumbnailUrl(user.value("avatar_url").toString());
    setDate(QDateTime::fromString(activity.value("created_at").toString(),
                                  "yyyy/MM/dd HH:mm:ss +0000").toString("dd MMM yyyy"));
    setDescription(track.value("description").toString());
    setId(activity.value("id").toString());
    setOriginId(track.value("id").toString());
    setOriginThumbnailUrl(track.value("artwork_url").toString());
    setTitle(track.value("title").toString());
}

void SoundCloudActivity::loadTrackSharingActivity(const QVariantMap &activity) {
    QVariantMap track = activity.value("origin").toMap().value("track").toMap();
    QVariantMap user = track.value("user").toMap();
    
    setArtist(user.value("username").toString());
    setArtistId(user.value("id").toString());
    setArtistThumbnailUrl(user.value("avatar_url").toString());
    setDate(QDateTime::fromString(activity.value("created_at").toString(),
                                  "yyyy/MM/dd HH:mm:ss +0000").toString("dd MMM yyyy"));
    setDescription(track.value("description").toString());
    setId(activity.value("id").toString());
    setOriginId(track.value("id").toString());
    setOriginThumbnailUrl(track.value("artwork_url").toString());
    setTitle(track.value("title").toString());
}

void SoundCloudActivity::loadActivity(SoundCloudActivity *activity) {
    setActivityType(activity->activityType());
    setArtist(activity->artist());
    setArtistId(activity->artistId());
    setArtistThumbnailUrl(activity->artistThumbnailUrl());
    setDate(activity->date());
    setDescription(activity->description());
    setId(activity->id());
    setOriginId(activity->originId());
    setOriginThumbnailUrl(activity->originThumbnailUrl());
    setTitle(activity->title());
}

void SoundCloudActivity::initRequest() {
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

void SoundCloudActivity::onActivityRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        loadActivity(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onActivityRequestFinished()));
    emit statusChanged(status());
}
