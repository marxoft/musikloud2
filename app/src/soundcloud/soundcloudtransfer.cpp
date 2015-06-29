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
#include "soundcloud.h"
#include <qsoundcloud/streamsrequest.h>

SoundCloudTransfer::SoundCloudTransfer(QObject *parent) :
    Transfer(parent),
    m_streamsRequest(0)
{
}

void SoundCloudTransfer::listStreams() {
    if (!m_streamsRequest) {
        m_streamsRequest = new QSoundCloud::StreamsRequest(this);
        m_streamsRequest->setClientId(SoundCloud::instance()->clientId());
        m_streamsRequest->setClientSecret(SoundCloud::instance()->clientSecret());
        m_streamsRequest->setAccessToken(SoundCloud::instance()->accessToken());
        m_streamsRequest->setRefreshToken(SoundCloud::instance()->refreshToken());
        
        connect(m_streamsRequest, SIGNAL(accessTokenChanged(QString)), SoundCloud::instance(), SLOT(setAccessToken(QString)));
        connect(m_streamsRequest, SIGNAL(refreshTokenChanged(QString)), SoundCloud::instance(), SLOT(setRefreshToken(QString)));
        connect(m_streamsRequest, SIGNAL(finished()), this, SLOT(onStreamsRequestFinished()));
    }
    
    m_streamsRequest->get(resourceId());
}

void SoundCloudTransfer::onStreamsRequestFinished() {
    if (m_streamsRequest->status() == QSoundCloud::StreamsRequest::Ready) {
        QVariantList list = m_streamsRequest->result().toList();
        
        foreach (QVariant v, list) {
            QVariantMap stream = v.toMap();
        
            if (stream.value("id") == streamId()) {
                QString ext = stream.value("ext").toString();
                
                if (!ext.isEmpty()) {
                    setFileExtension(ext);
                }
                
                startDownload(stream.value("url").toString());
                return;
            }
        }
    }
    
    setErrorString(tr("No stream URL found"));
    setStatus(Failed);
}
