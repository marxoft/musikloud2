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

#include "pluginartist.h"
#include "pluginmanager.h"
#include "resources.h"

PluginArtist::PluginArtist(QObject *parent) :
    MKArtist(parent),
    m_request(0)
{
}

PluginArtist::PluginArtist(const QString &service, const QString &id, QObject *parent) :
    MKArtist(parent),
    m_request(0)
{
    loadArtist(service, id);
}

PluginArtist::PluginArtist(const QString &service, const QVariantMap &artist, QObject *parent) :
    MKArtist(parent),
    m_request(0)
{
    loadArtist(service, artist);
}

PluginArtist::PluginArtist(const PluginArtist *artist, QObject *parent) :
    MKArtist(artist, parent),
    m_request(0),
    m_actions(artist->actions()),
    m_playlistsId(artist->playlistsId()),
    m_tracksId(artist->tracksId())
{
}

QVariantList PluginArtist::actions() const {
    return m_actions;
}

void PluginArtist::setActions(const QVariantList &a) {
    m_actions = a;
    emit changed();
    emit actionsChanged();
}

QString PluginArtist::playlistsId() const {
    return m_playlistsId;
}

void PluginArtist::setPlaylistsId(const QString &i) {
    if (i != playlistsId()) {
        m_playlistsId = i;
        emit changed();
        emit playlistsIdChanged();
    }
}

QString PluginArtist::tracksId() const {
    return m_tracksId;
}

void PluginArtist::setTracksId(const QString &i) {
    if (i != tracksId()) {
        m_tracksId = i;
        emit changed();
        emit tracksIdChanged();
    }
}

QString PluginArtist::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginArtist::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

void PluginArtist::loadArtist(const QString &service, const QString &id) {
    setService(service);
    setId(id);
    
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->get(Resources::ARTIST, id);
        emit changed();
        emit statusChanged(status());
    }
}

void PluginArtist::loadArtist(const QString &service, const QVariantMap &artist) {
    setService(service);
    
    if (artist.contains("actions")) {
        setActions(artist.value("actions").toList());
    }
    
    if (artist.contains("description")) {
        setDescription(artist.value("description").toString());
    }
    
    if (artist.contains("id")) {
        setId(artist.value("id").toString());
    }
    
    if (artist.contains("largeThumbnailUrl")) {
        setLargeThumbnailUrl(artist.value("largeThumbnailUrl").toString());
    }
    
    if (artist.contains("name")) {
        setName(artist.value("name").toString());
    }
    
    if (artist.contains("playlistsId")) {
        setPlaylistsId(artist.value("playlistsId").toString());
    }
    
    if (artist.contains("thumbnailUrl")) {
        setThumbnailUrl(artist.value("thumbnailUrl").toString());
    }
    
    if (artist.contains("tracksId")) {
        setTracksId(artist.value("tracksId").toString());
    }

    if (artist.contains("url")) {
        setUrl(artist.value("url").toString());
    }
}

void PluginArtist::loadArtist(PluginArtist *artist) {
    MKArtist::loadArtist(artist);
    setActions(artist->actions());
    setPlaylistsId(artist->playlistsId());
    setTracksId(artist->tracksId());
}

void PluginArtist::cancel() {
    if (status() == ResourcesRequest::Loading) {
        m_request->cancel();
        emit changed();
        emit statusChanged(status());
    }
}

void PluginArtist::del(const QString &resourceType, const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->del(Resources::ARTIST, id(), resourceType, resourceId);
        emit changed();
        emit statusChanged(status());
    }
}

void PluginArtist::insert(const QString &resourceType, const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->insert(Resources::ARTIST, id(), resourceType, resourceId);
        emit changed();
        emit statusChanged(status());
    }
}

ResourcesRequest* PluginArtist::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginArtist::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            loadArtist(service(), result);
        }
    }
    
    emit changed();
    emit statusChanged(status());
}
