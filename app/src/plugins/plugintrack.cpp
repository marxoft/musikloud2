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

#include "plugintrack.h"
#include "pluginmanager.h"
#include "resources.h"

PluginTrack::PluginTrack(QObject *parent) :
    MKTrack(parent),
    m_request(0)
{
}

PluginTrack::PluginTrack(const QString &service, const QString &id, QObject *parent) :
    MKTrack(parent),
    m_request(0)
{
    loadTrack(service, id);
}

PluginTrack::PluginTrack(const QString &service, const QVariantMap &track, QObject *parent) :
    MKTrack(parent),
    m_request(0)
{
    loadTrack(service, track);
}

PluginTrack::PluginTrack(const PluginTrack *track, QObject *parent) :
    MKTrack(track, parent),
    m_request(0),
    m_actions(track->actions()),
    m_commentsId(track->commentsId()),
    m_relatedTracksId(track->relatedTracksId())
{
}

QVariantList PluginTrack::actions() const {
    return m_actions;
}

void PluginTrack::setActions(const QVariantList &a) {
    m_actions = a;
    emit changed();
    emit actionsChanged();
}

QString PluginTrack::commentsId() const {
    return m_commentsId;
}

void PluginTrack::setCommentsId(const QString &i) {
    if (i != commentsId()) {
        m_commentsId = i;
        emit changed();
        emit commentsIdChanged();
    }
}

QString PluginTrack::relatedTracksId() const {
    return m_relatedTracksId;
}

void PluginTrack::setRelatedTracksId(const QString &i) {
    if (i != relatedTracksId()) {
        m_relatedTracksId = i;
        emit changed();
        emit relatedTracksIdChanged();
    }
}

QString PluginTrack::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

ResourcesRequest::Status PluginTrack::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

void PluginTrack::loadTrack(const QString &service, const QString &id) {
    setService(service);
    setId(id);
    
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->get(Resources::TRACK, id);
        emit changed();
        emit statusChanged(status());
    }
}

void PluginTrack::loadTrack(const QString &service, const QVariantMap &track) {
    setService(service);
    
    if (track.contains("actions")) {
        setActions(track.value("actions").toList());
    }
    
    if (track.contains("artist")) {
        setArtist(track.value("artist").toString());
    }
    
    if (track.contains("artistId")) {
        setArtistId(track.value("artistId").toString());
    }
    
    if (track.contains("commentsId")) {
        setCommentsId(track.value("commentsId").toString());
    }
    
    if (track.contains("date")) {
        setDate(track.value("date").toString());
    }
    
    if (track.contains("description")) {
        setDescription(track.value("description").toString());
    }
    
    if (track.contains("downloadable")) {
        setDownloadable(track.value("downloadable").toBool());
    }

    if (track.contains("duration")) {
        if (track.value("duration").type() == QVariant::String) {
            setDurationString(track.value("duration").toString());
        }
        else {
            setDuration(track.value("duration").toLongLong());
        }
    }
    
    if (track.contains("format")) {
        setFormat(track.value("format").toString());
    }
    
    if (track.contains("genre")) {
        setGenre(track.value("genre").toString());
    }
    
    if (track.contains("id")) {
        setId(track.value("id").toString());
    }
    
    if (track.contains("largeThumbnailUrl")) {
        setLargeThumbnailUrl(track.value("largeThumbnailUrl").toString());
    }
    
    if (track.contains("playCount")) {
        setPlayCount(track.value("playCount").toLongLong());
    }
    
    if (track.contains("relatedTracksId")) {
        setRelatedTracksId(track.value("relatedTracksId").toString());
    }

    if (track.contains("size")) {
        if (track.value("size").type() == QVariant::String) {
            setSizeString(track.value("size").toString());
        }
        else {
            setSize(track.value("size").toLongLong());
        }
    }
    
    if (track.contains("streamUrl")) {
        setStreamUrl(track.value("streamUrl").toString());
    }
    
    if (track.contains("thumbnailUrl")) {
        setThumbnailUrl(track.value("thumbnailUrl").toString());
    }
    
    if (track.contains("title")) {
        setTitle(track.value("title").toString());
    }
    
    if (track.contains("url")) {
        setUrl(track.value("url").toString());
    }
}

void PluginTrack::loadTrack(PluginTrack *track) {
    MKTrack::loadTrack(track);
    setActions(track->actions());
    setCommentsId(track->commentsId());
    setRelatedTracksId(track->relatedTracksId());
}

void PluginTrack::cancel() {
    if (status() == ResourcesRequest::Loading) {
        m_request->cancel();
        emit changed();
        emit statusChanged(status());
    }
}

void PluginTrack::del(const QString &resourceType, const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->del(Resources::TRACK, id(), resourceType, resourceId);
        emit changed();
        emit statusChanged(status());
    }
}

void PluginTrack::insert(const QString &resourceType, const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->insert(Resources::TRACK, id(), resourceType, resourceId);
        emit changed();
        emit statusChanged(status());
    }
}

ResourcesRequest* PluginTrack::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginTrack::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            loadTrack(service(), result);
        }
    }
    
    emit changed();
    emit statusChanged(status());
}
