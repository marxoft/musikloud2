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

#include "soundcloudtransfer.h"
#include "logger.h"
#include "soundcloud.h"
#include <qsoundcloud/streamsrequest.h>
#include <QFileInfo>

SoundCloudTransfer::SoundCloudTransfer(QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0)
{
}

void SoundCloudTransfer::listStreams() {
    if (!m_streamsRequest) {
        m_streamsRequest = new QSoundCloud::StreamsRequest(this);
        m_streamsRequest->setClientId(SoundCloud::clientId());
        m_streamsRequest->setClientSecret(SoundCloud::clientSecret());
        m_streamsRequest->setAccessToken(SoundCloud::accessToken());
        m_streamsRequest->setRefreshToken(SoundCloud::refreshToken());
        
        connect(m_streamsRequest, SIGNAL(accessTokenChanged(QString)), SoundCloud::instance(), SLOT(setAccessToken(QString)));
        connect(m_streamsRequest, SIGNAL(refreshTokenChanged(QString)), SoundCloud::instance(), SLOT(setRefreshToken(QString)));
        connect(m_streamsRequest, SIGNAL(finished()), this, SLOT(onStreamsRequestFinished()));
    }
    
    m_streamsRequest->get(trackId());
}

void SoundCloudTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == QSoundCloud::StreamsRequest::Ready) {
        const QVariantList list = m_streamsRequest->result().toList();
        
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
    else {
        Logger::log("SoundCloudTransfer::onStreamsRequestFinished(). Error: " + m_streamsRequest->errorString());
    }
    
    setErrorString(tr("No stream URL found"));
    setStatus(Failed);
}
