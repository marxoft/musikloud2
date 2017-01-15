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

#include "plugintransfer.h"
#include "logger.h"
#include "pluginmanager.h"
#include "resources.h"
#include "resourcesrequest.h"
#include <qsoundcloud/streamsrequest.h>
#include <QFileInfo>

PluginTransfer::PluginTransfer(const QString &service, QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0),
    m_soundcloudStreamsRequest(0)
{
    setService(service);
}

void PluginTransfer::listStreams() {
    if (ResourcesRequest *r = streamsRequest()) {
        Logger::log("PluginTransfer::listStreams(). ID: " + trackId(), Logger::MediumVerbosity);
        r->list(Resources::STREAM, trackId());
    }
    else {
        setErrorString(tr("No streams plugin found for service '%1'").arg(service()));
        Logger::log("PluginTransfer::listStreams(). Error: " + errorString());
        setStatus(Failed);
    }
}

ResourcesRequest* PluginTransfer::streamsRequest() {
    if (!m_streamsRequest) {
        m_streamsRequest = PluginManager::instance()->createRequestForService(service(), this);

        if (m_streamsRequest) {
            connect(m_streamsRequest, SIGNAL(finished()), this, SLOT(onStreamsRequestFinished()));
        }
    }

    return m_streamsRequest;
}

QSoundCloud::StreamsRequest* PluginTransfer::soundcloudStreamsRequest() {
    if (!m_soundcloudStreamsRequest) {
        m_soundcloudStreamsRequest = new QSoundCloud::StreamsRequest(this);
        connect(m_soundcloudStreamsRequest, SIGNAL(finished()), this, SLOT(onSoundCloudStreamsRequestFinished()));
    }
    
    return m_soundcloudStreamsRequest;
}

void PluginTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_streamsRequest->result().toMap();
        
        if (result.contains("items")) {
            const QVariantList list = result.value("items").toList();
            
            foreach (const QVariant &v, list) {
                const QVariantMap stream = v.toMap();
                
                if (stream.value("id") == streamId()) {
                    const QFileInfo info(downloadPath() + fileName());
                    QString suffix = info.suffix();
                    
                    if (suffix.isEmpty()) {
                        suffix = stream.value("ext").toString();
                        
                        if (!suffix.isEmpty()) {
                            if (!suffix.startsWith(".")) {
                                suffix.prepend(".");
                            }
                            
                            setFileName(info.completeBaseName() + suffix);
                        }
                    }
                    
                    startDownload(stream.value("url").toString());
                    return;
                }
            }
        }
        else if (result.contains("service")) {
            const QString service = result.value("service").toString();
            
            if (service == Resources::SOUNDCLOUD) {
                soundcloudStreamsRequest()->get(result.value("id").toString());
            }
            else {
                setErrorString(tr("Attempted redirect to unsupported service '%1'").arg(service));
                Logger::log("PluginTransfer::onStreamsRequestFinished(). Error: " + errorString());
                setStatus(Failed);
            }
            
            return;
        }
    }
    
    setErrorString(tr("No stream URL found"));
    Logger::log("PluginTransfer::onStreamsRequestFinished(). Error: " + errorString());
    setStatus(Failed);
}

void PluginTransfer::onSoundCloudStreamsRequestFinished() {
    if (m_soundcloudStreamsRequest->status() == QSoundCloud::StreamsRequest::Ready) {
        const QVariantList list = m_soundcloudStreamsRequest->result().toList();
        
        foreach (const QVariant &v, list) {
            const QVariantMap stream = v.toMap();
            
            if (stream.value("id") == streamId()) {
                const QFileInfo info(downloadPath() + fileName());
                QString suffix = info.suffix();
                
                if (suffix.isEmpty()) {
                    suffix = stream.value("ext").toString();
                    
                    if (!suffix.isEmpty()) {
                        if (!suffix.startsWith(".")) {
                            suffix.prepend(".");
                        }
                        
                        setFileName(info.completeBaseName() + suffix);
                    }
                }
                
                startDownload(stream.value("url").toString());
                return;
            }
        }
    }
    
    setErrorString(tr("No stream URL found"));
    Logger::log("PluginTransfer::onSoundCloudStreamsRequestFinished(). Error: " + errorString());
    setStatus(Failed);
}
