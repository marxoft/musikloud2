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
#include "resources.h"

PluginArtist::PluginArtist(QObject *parent) :
    MKArtist(parent),
    m_request(new ResourcesRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginArtist::PluginArtist(const QString &service, const QString &id, QObject *parent) :
    MKArtist(parent),
    m_request(new ResourcesRequest(this))
{
    loadArtist(service, id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginArtist::PluginArtist(const QString &service, const QVariantMap &artist, QObject *parent) :
    MKArtist(parent),
    m_request(new ResourcesRequest(this))
{
    loadArtist(service, artist);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginArtist::PluginArtist(PluginArtist *artist, QObject *parent) :
    MKArtist(artist, parent),
    m_request(new ResourcesRequest(this))
{
}

QString PluginArtist::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginArtist::status() const {
    return m_request->status();
}

void PluginArtist::loadArtist(const QString &service, const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    m_request->setService(service);
    m_request->get(Resources::ARTIST, id);

    emit statusChanged(status());
}

void PluginArtist::loadArtist(const QString &service, const QVariantMap &artist) {
    setService(service);
    setDescription(artist.value("description").toString());
    setId(artist.value("id").toString());
    setLargeThumbnailUrl(artist.value("largeThumbnailUrl").toString());
    setName(artist.value("name").toString());
    setThumbnailUrl(artist.value("thumbnailUrl").toString());
}

void PluginArtist::loadArtist(PluginArtist *artist) {
    MKArtist::loadArtist(artist);
}

void PluginArtist::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        loadArtist(m_request->service(), m_request->result().toMap());
    }
    
    emit statusChanged(status());
}
