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

#include "pluginplaylist.h"
#include "pluginmanager.h"
#include "resources.h"

PluginPlaylist::PluginPlaylist(QObject *parent) :
    MKPlaylist(parent),
    m_request(0)
{
}

PluginPlaylist::PluginPlaylist(const QString &service, const QString &id, QObject *parent) :
    MKPlaylist(parent),
    m_request(0)
{
    loadPlaylist(service, id);
}

PluginPlaylist::PluginPlaylist(const QString &service, const QVariantMap &playlist, QObject *parent) :
    MKPlaylist(parent),
    m_request(0)
{
    loadPlaylist(service, playlist);
}

PluginPlaylist::PluginPlaylist(const PluginPlaylist *playlist, QObject *parent) :
    MKPlaylist(playlist, parent),
    m_request(0),
    m_actions(playlist->actions()),
    m_tracksId(playlist->tracksId())
{
}

QVariantList PluginPlaylist::actions() const {
    return m_actions;
}

void PluginPlaylist::setActions(const QVariantList &a) {
    m_actions = a;
    emit changed();
    emit actionsChanged();
}

QString PluginPlaylist::tracksId() const {
    return m_tracksId;
}

void PluginPlaylist::setTracksId(const QString &i) {
    if (i != tracksId()) {
        m_tracksId = i;
        emit changed();
        emit tracksIdChanged();
    }
}

QString PluginPlaylist::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

ResourcesRequest::Status PluginPlaylist::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

void PluginPlaylist::loadPlaylist(const QString &service, const QString &id) {
    setService(service);
    setId(id);
    
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->get(Resources::PLAYLIST, id);
        emit changed();
        emit statusChanged(status());
    }
}

void PluginPlaylist::loadPlaylist(const QString &service, const QVariantMap &playlist) {
    setService(service);
    
    if (playlist.contains("actions")) {
        setActions(playlist.value("actions").toList());
    }
    
    if (playlist.contains("artist")) {
        setArtist(playlist.value("artist").toString());
    }
    
    if (playlist.contains("artistId")) {
        setArtistId(playlist.value("artistId").toString());
    }
    
    if (playlist.contains("date")) {
        setDate(playlist.value("date").toString());
    }
    
    if (playlist.contains("description")) {
        setDescription(playlist.value("description").toString());
    }

    if (playlist.contains("duration")) {
        if (playlist.value("duration").type() == QVariant::String) {
            setDurationString(playlist.value("duration").toString());
        }
        else {
            setDuration(playlist.value("duration").toLongLong());
        }
    }
    
    if (playlist.contains("genre")) {
        setGenre(playlist.value("genre").toString());
    }
    
    if (playlist.contains("id")) {
        setId(playlist.value("id").toString());
    }
    
    if (playlist.contains("largeThumbnailUrl")) {
        setLargeThumbnailUrl(playlist.value("largeThumbnailUrl").toString());
    }
    
    if (playlist.contains("thumbnailUrl")) {
        setThumbnailUrl(playlist.value("thumbnailUrl").toString());
    }
    
    if (playlist.contains("title")) {
        setTitle(playlist.value("title").toString());
    }
    
    if (playlist.contains("trackCount")) {
        setTrackCount(playlist.value("trackCount").toInt());
    }
    
    if (playlist.contains("tracksId")) {
        setTracksId(playlist.value("tracksId").toString());
    }

    if (playlist.contains("url")) {
        setUrl(playlist.value("url").toString());
    }
}

void PluginPlaylist::loadPlaylist(PluginPlaylist *playlist) {
    MKPlaylist::loadPlaylist(playlist);
    setActions(playlist->actions());
    setTracksId(playlist->tracksId());
}

void PluginPlaylist::cancel() {
    if (status() == ResourcesRequest::Loading) {
        m_request->cancel();
        emit changed();
        emit statusChanged(status());
    }
}

void PluginPlaylist::del(const QString &resourceType, const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->del(Resources::PLAYLIST, id(), resourceType, resourceId);
        emit changed();
        emit statusChanged(status());
    }
}

void PluginPlaylist::insert(const QString &resourceType, const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->insert(Resources::PLAYLIST, id(), resourceType, resourceId);
        emit changed();
        emit statusChanged(status());
    }
}

ResourcesRequest* PluginPlaylist::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginPlaylist::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            loadPlaylist(service(), result);
        }
    }
    
    emit changed();
    emit statusChanged(status());
}
