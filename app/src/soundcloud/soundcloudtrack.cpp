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

#include "soundcloudtrack.h"
#include "definitions.h"
#include "logger.h"
#include "resources.h"
#include "soundcloud.h"
#include "utils.h"
#include <QDateTime>

SoundCloudTrack::SoundCloudTrack(QObject *parent) :
    MKTrack(parent),
    m_request(0),
    m_commentable(true),
    m_favourite(false),
    m_favouriteCount(0)
{
    setService(Resources::SOUNDCLOUD);
    connect(SoundCloud::instance(), SIGNAL(trackFavourited(SoundCloudTrack*)),
            this, SLOT(onTrackUpdated(SoundCloudTrack*)));
    connect(SoundCloud::instance(), SIGNAL(trackUnfavourited(SoundCloudTrack*)),
            this, SLOT(onTrackUpdated(SoundCloudTrack*)));
}

SoundCloudTrack::SoundCloudTrack(const QString &id, QObject *parent) :
    MKTrack(parent),
    m_request(0),
    m_commentable(true),
    m_favourite(false),
    m_favouriteCount(0)
{
    setService(Resources::SOUNDCLOUD);
    loadTrack(id);
    connect(SoundCloud::instance(), SIGNAL(trackFavourited(SoundCloudTrack*)),
            this, SLOT(onTrackUpdated(SoundCloudTrack*)));
    connect(SoundCloud::instance(), SIGNAL(trackUnfavourited(SoundCloudTrack*)),
            this, SLOT(onTrackUpdated(SoundCloudTrack*)));
}

SoundCloudTrack::SoundCloudTrack(const QVariantMap &track, QObject *parent) :
    MKTrack(parent),
    m_request(0),
    m_commentable(true),
    m_favourite(false),
    m_favouriteCount(0)
{
    setService(Resources::SOUNDCLOUD);
    loadTrack(track);
    connect(SoundCloud::instance(), SIGNAL(trackFavourited(SoundCloudTrack*)),
            this, SLOT(onTrackUpdated(SoundCloudTrack*)));
    connect(SoundCloud::instance(), SIGNAL(trackUnfavourited(SoundCloudTrack*)),
            this, SLOT(onTrackUpdated(SoundCloudTrack*)));
}

SoundCloudTrack::SoundCloudTrack(const SoundCloudTrack *track, QObject *parent) :
    MKTrack(track, parent),
    m_request(0),
    m_commentable(track->isCommentable()),
    m_favourite(track->isFavourite()),
    m_favouriteCount(track->favouriteCount()),
    m_sharing(track->sharing()),
    m_waveformUrl(track->waveformUrl())
{
    connect(SoundCloud::instance(), SIGNAL(trackFavourited(SoundCloudTrack*)),
            this, SLOT(onTrackUpdated(SoundCloudTrack*)));
    connect(SoundCloud::instance(), SIGNAL(trackUnfavourited(SoundCloudTrack*)),
            this, SLOT(onTrackUpdated(SoundCloudTrack*)));
}

bool SoundCloudTrack::isCommentable() const {
    return m_commentable;
}

void SoundCloudTrack::setCommentable(bool c) {
    if (c != isCommentable()) {
        m_commentable = c;
        emit changed();
        emit commentableChanged();
    }
}

QString SoundCloudTrack::errorString() const {
    return m_request ? SoundCloud::getErrorString(m_request->result().toMap()) : QString();
}

bool SoundCloudTrack::isFavourite() const {
    return m_favourite;
}

void SoundCloudTrack::setFavourite(bool f) {
    if (f != isFavourite()) {
        m_favourite = f;
        emit changed();
        emit favouriteChanged();
    }
}

qint64 SoundCloudTrack::favouriteCount() const {
    return m_favouriteCount;
}

void SoundCloudTrack::setFavouriteCount(qint64 c) {
    if (c != favouriteCount()) {
        m_favouriteCount = c;
        emit changed();
        emit favouriteCountChanged();
    }
}

QString SoundCloudTrack::sharing() const {
    return m_sharing;
}

void SoundCloudTrack::setSharing(const QString &s) {
    if (s != sharing()) {
        m_sharing = s;
        emit changed();
        emit sharingChanged();
    }
}

QSoundCloud::ResourcesRequest::Status SoundCloudTrack::status() const {
    return m_request ? m_request->status() : QSoundCloud::ResourcesRequest::Null;
}

QUrl SoundCloudTrack::waveformUrl() const {
    return m_waveformUrl;
}

void SoundCloudTrack::setWaveformUrl(const QUrl &u) {
    if (u != waveformUrl()) {
        m_waveformUrl = u;
        emit changed();
        emit waveformUrlChanged();
    }
}

void SoundCloudTrack::loadTrack(const QString &id) {
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
        m_request->get("/tracks/" + id);
    }
    
    connect(m_request, SIGNAL(finished()), this, SLOT(onTrackRequestFinished()));
    emit changed();
    emit statusChanged(status());
}

void SoundCloudTrack::loadTrack(const QVariantMap &track) {
    const QVariantMap user = track.value("user").toMap();
    const QString thumbnail = track.value("artwork_url").toString();
    
    setArtist(user.value("username").toString());
    setArtistId(user.value("id").toString());
    setCommentable(track.value("commentable").toBool());
    setDate(QDateTime::fromString(track.value("created_at").toString(),
                                  "yyyy/MM/dd HH:mm:ss +0000").toString("dd MMM yyyy"));
    setDescription(track.value("description").toString());
    setDuration(track.value("duration").toLongLong());
    setFavourite(track.value("user_favorite").toBool());
    setFavouriteCount(track.value("favoritings_count").toLongLong());
    setFormat(track.value("original_format").toString().toUpper());
    setGenre(track.value("genre").toString());
    setId(track.value("id").toString());
    setPlayCount(track.value("playback_count").toLongLong());
    setSize(track.value("original_content_size").toLongLong());
    setTitle(track.value("title").toString());
    setUrl(track.value("permalink_url").toString());
    setWaveformUrl(track.value("waveform_url").toString());

    if (!thumbnail.isEmpty()) {
        setLargeThumbnailUrl(QString("%1-t%2x%2.jpg").arg(thumbnail.left(thumbnail.lastIndexOf('-'))).arg(LARGE_THUMBNAIL_SIZE));
        setThumbnailUrl(QString("%1-t%2x%2.jpg").arg(thumbnail.left(thumbnail.lastIndexOf('-'))).arg(THUMBNAIL_SIZE));
    }
    else {
        setLargeThumbnailUrl(QString());
        setThumbnailUrl(QString());
    }
}

void SoundCloudTrack::loadTrack(SoundCloudTrack *track) {
    MKTrack::loadTrack(track);
    setCommentable(track->isCommentable());
    setFavourite(track->isFavourite());
    setFavouriteCount(track->favouriteCount());
    setWaveformUrl(track->waveformUrl());
}
    
void SoundCloudTrack::favourite() {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("SoundCloudTrack::favourite(). ID: " + id(), Logger::MediumVerbosity);
    initRequest();
    m_request->insert("/me/favorites/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onFavouriteRequestFinished()));
    emit changed();
    emit statusChanged(status());
}

void SoundCloudTrack::unfavourite() {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("SoundCloudTrack::unfavourite(). ID: " + id(), Logger::MediumVerbosity);
    initRequest();
    m_request->del("/me/favorites/" + id());
    connect(m_request, SIGNAL(finished()), this, SLOT(onUnfavouriteRequestFinished()));
    emit changed();
    emit statusChanged(status());
}

void SoundCloudTrack::cancel() {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        m_request->cancel();
        emit changed();
        emit statusChanged(status());
    }
}

void SoundCloudTrack::initRequest() {
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

void SoundCloudTrack::onTrackRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        loadTrack(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onTrackRequestFinished()));
    emit changed();
    emit statusChanged(status());
}

void SoundCloudTrack::onFavouriteRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        setFavourite(true);
        setFavouriteCount(favouriteCount() + 1);
        Logger::log("SoundCloudTrack::onFavouriteRequestFinished(). Video favourited. ID: " + id(),
                    Logger::MediumVerbosity);
        emit SoundCloud::instance()->trackFavourited(this);
    }
    else {
        Logger::log("SoundCloudTrack::onFavouriteRequestFinished(). Error: " + errorString());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onFavouriteRequestFinished()));
    emit changed();
    emit statusChanged(status());
}

void SoundCloudTrack::onUnfavouriteRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        setFavourite(false);
        setFavouriteCount(favouriteCount() - 1);
        Logger::log("SoundCloudTrack::onUnfavouriteRequestFinished(). Video unfavourited. ID: " + id(),
                    Logger::MediumVerbosity);
        emit SoundCloud::instance()->trackUnfavourited(this);
    }
    else {
        Logger::log("SoundCloudTrack::onUnfavouriteRequestFinished(). Error: " + errorString());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onUnfavouriteRequestFinished()));
    emit changed();
    emit statusChanged(status());
}

void SoundCloudTrack::onTrackUpdated(SoundCloudTrack *track) {
    if ((track->id() == id()) && (track != this)) {
        loadTrack(track);
    }
}
