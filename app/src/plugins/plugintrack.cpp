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
#include "resources.h"

PluginTrack::PluginTrack(QObject *parent) :
    MKTrack(parent),
    m_request(new ResourcesRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginTrack::PluginTrack(const QString &service, const QString &id, QObject *parent) :
    MKTrack(parent),
    m_request(new ResourcesRequest(this))
{
    loadTrack(service, id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginTrack::PluginTrack(const QString &service, const QVariantMap &track, QObject *parent) :
    MKTrack(parent),
    m_request(new ResourcesRequest(this))
{
    loadTrack(service, track);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginTrack::PluginTrack(PluginTrack *track, QObject *parent) :
    MKTrack(track, parent),
    m_request(new ResourcesRequest(this))
{
}

QString PluginTrack::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginTrack::status() const {
    return m_request->status();
}

void PluginTrack::loadTrack(const QString &service, const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    m_request->setService(service);
    m_request->get(Resources::TRACK, id);

    emit statusChanged(status());
}

void PluginTrack::loadTrack(const QString &service, const QVariantMap &track) {
    setService(service);
    setArtist(track.value("artist").toString());
    setArtistId(track.value("artistId").toString());
    setDate(track.value("date").toString());
    setDescription(track.value("description").toString());
    setDownloadable(track.value("downloadable", true).toBool());
    setFormat(track.value("format").toString());
    setGenre(track.value("genre").toString());
    setId(track.value("id").toString());
    setLargeThumbnailUrl(track.value("largeThumbnailUrl").toString());
    setPlayCount(track.value("playCount").toLongLong());
    setStreamUrl(track.value("streamUrl").toString());
    setThumbnailUrl(track.value("thumbnailUrl").toString());
    setTitle(track.value("title").toString());
    setUrl(track.value("url").toString());
        
    if (track.value("duration").type() == QVariant::String) {
        setDurationString(track.value("duration").toString());
    }
    else {
        setDuration(track.value("duration").toLongLong());
    }
    
    if (track.value("size").type() == QVariant::String) {
        setSizeString(track.value("size").toString());
    }
    else {
        setSize(track.value("size").toLongLong());
    }
}

void PluginTrack::loadTrack(PluginTrack *track) {
    MKTrack::loadTrack(track);
}

void PluginTrack::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        loadTrack(m_request->service(), m_request->result().toMap());
    }
    
    emit statusChanged(status());
}
