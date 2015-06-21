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
#include "resources.h"

PluginPlaylist::PluginPlaylist(QObject *parent) :
    MKPlaylist(parent),
    m_request(new ResourcesRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginPlaylist::PluginPlaylist(const QString &service, const QString &id, QObject *parent) :
    MKPlaylist(parent),
    m_request(new ResourcesRequest(this))
{
    loadPlaylist(service, id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginPlaylist::PluginPlaylist(const QString &service, const QVariantMap &playlist, QObject *parent) :
    MKPlaylist(parent),
    m_request(new ResourcesRequest(this))
{
    loadPlaylist(service, playlist);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginPlaylist::PluginPlaylist(PluginPlaylist *playlist, QObject *parent) :
    MKPlaylist(playlist, parent),
    m_request(new ResourcesRequest(this))
{
}

QString PluginPlaylist::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginPlaylist::status() const {
    return m_request->status();
}

void PluginPlaylist::loadPlaylist(const QString &service, const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    m_request->setService(service);
    m_request->get(Resources::PLAYLIST, id);

    emit statusChanged(status());
}

void PluginPlaylist::loadPlaylist(const QString &service, const QVariantMap &playlist) {
    setService(service);
    setArtist(playlist.value("artist").toString());
    setArtistId(playlist.value("artistId").toString());
    setDate(playlist.value("date").toString());
    setDescription(playlist.value("description").toString());
    setGenre(playlist.value("genre").toString());
    setId(playlist.value("id").toString());
    setLargeThumbnailUrl(playlist.value("largeThumbnailUrl").toString());
    setThumbnailUrl(playlist.value("thumbnailUrl").toString());
    setTitle(playlist.value("title").toString());
    setTrackCount(playlist.value("trackCount").toInt());
    
     if (playlist.value("duration").type() == QVariant::String) {
        setDurationString(playlist.value("duration").toString());
    }
    else {
        setDuration(playlist.value("duration").toLongLong());
    }
}

void PluginPlaylist::loadPlaylist(PluginPlaylist *playlist) {
    MKPlaylist::loadPlaylist(playlist);
}

void PluginPlaylist::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        loadPlaylist(m_request->service(), m_request->result().toMap());
    }
    
    emit statusChanged(status());
}
