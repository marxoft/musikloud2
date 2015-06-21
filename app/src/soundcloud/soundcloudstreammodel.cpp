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

#include "soundcloudstreammodel.h"
#include "soundcloud.h"

SoundCloudStreamModel::SoundCloudStreamModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QSoundCloud::StreamsRequest(this))
{
    m_request->setClientId(SoundCloud::instance()->clientId());
    m_request->setClientSecret(SoundCloud::instance()->clientSecret());
    m_request->setAccessToken(SoundCloud::instance()->accessToken());
    m_request->setRefreshToken(SoundCloud::instance()->refreshToken());
        
    connect(m_request, SIGNAL(accessTokenChanged(QString)), SoundCloud::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(refreshTokenChanged(QString)), SoundCloud::instance(), SLOT(setRefreshToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString SoundCloudStreamModel::errorString() const {
    return m_request->errorString();
}

QSoundCloud::StreamsRequest::Status SoundCloudStreamModel::status() const {
    return m_request->status();
}

void SoundCloudStreamModel::get(const QString &id) {
    if (status() == QSoundCloud::StreamsRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_request->get(id);
    emit statusChanged(status());
}

void SoundCloudStreamModel::cancel() {
    m_request->cancel();
}

void SoundCloudStreamModel::reload() {
    clear();
    m_request->get(m_id);
    emit statusChanged(status());
}

void SoundCloudStreamModel::onRequestFinished() {
    if (m_request->status() == QSoundCloud::StreamsRequest::Ready) {
        foreach (QVariant v, m_request->result().toList()) {
            QVariantMap stream = v.toMap();
            append(stream.value("description").toString(), stream);
        }
    }
    
    emit statusChanged(status());
}
