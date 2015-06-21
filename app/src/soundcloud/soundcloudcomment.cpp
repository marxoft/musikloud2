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

#include "soundcloudcomment.h"
#include "resources.h"
#include "soundcloud.h"
#include <QDateTime>
#ifdef MUSIKLOUD_DEBUG
#include <QDebug>
#endif

SoundCloudComment::SoundCloudComment(QObject *parent) :
    MKComment(parent),
    m_request(0)
{
    setService(Resources::SOUNDCLOUD);
}

SoundCloudComment::SoundCloudComment(const QString &id, QObject *parent) :
    MKComment(parent),
    m_request(0)
{
    setService(Resources::SOUNDCLOUD);
    loadComment(id);
}

SoundCloudComment::SoundCloudComment(const QVariantMap &comment, QObject *parent) :
    MKComment(parent),
    m_request(0)
{
    setService(Resources::SOUNDCLOUD);
    loadComment(comment);
}

SoundCloudComment::SoundCloudComment(SoundCloudComment *comment, QObject *parent) :
    MKComment(comment, parent),
    m_request(0)
{
}

QString SoundCloudComment::errorString() const {
    return m_request ? SoundCloud::getErrorString(m_request->result().toMap()) : QString();
}

QSoundCloud::ResourcesRequest::Status SoundCloudComment::status() const {
    return m_request ? m_request->status() : QSoundCloud::ResourcesRequest::Null;
}

void SoundCloudComment::loadComment(const QString &id) {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    m_request->get("/comments/" + id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onCommentRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudComment::loadComment(const QVariantMap &comment) {
    QVariantMap user = comment.value("user").toMap();
    
    setArtist(user.value("username").toString());
    setArtistId(user.value("id").toString());
    setBody(comment.value("body").toString());
    setDate(QDateTime::fromString(comment.value("created_at").toString(),
                                  "yyyy/MM/dd HH:mm:ss +0000").toString("dd MMM yyyy"));
    setId(comment.value("id").toString());
    setThumbnailUrl(user.value("avatar_url").toString());
    setTrackId(comment.value("track_id").toString());
}

void SoundCloudComment::addComment() {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    initRequest();
    
    QVariantMap resource;
    resource["body"] = body();
    m_request->insert(resource, QString("/tracks/%1/comments").arg(trackId()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onAddCommentRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudComment::addComment(const QVariantMap &comment) {
    loadComment(comment);
    addComment();
}

void SoundCloudComment::initRequest() {
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

void SoundCloudComment::onCommentRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        loadComment(m_request->result().toMap());
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onCommentRequestFinished()));
    emit statusChanged(status());
}

void SoundCloudComment::onAddCommentRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        loadComment(m_request->result().toMap());
        emit SoundCloud::instance()->commentAdded(this);
#ifdef MUSIKLOUD_DEBUG
        qDebug() << "SoundCloudComment::onAddCommentRequestFinished OK" << trackId();
#endif
    }
    
    disconnect(m_request, SIGNAL(finished()), this, SLOT(onAddCommentRequestFinished()));
    emit statusChanged(status());
}
