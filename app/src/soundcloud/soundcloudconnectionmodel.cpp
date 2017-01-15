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

#include "soundcloudconnectionmodel.h"
#include "logger.h"
#include "soundcloud.h"

SoundCloudConnectionModel::SoundCloudConnectionModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QSoundCloud::ResourcesRequest(this))
{
    m_request->setClientId(SoundCloud::clientId());
    m_request->setClientSecret(SoundCloud::clientSecret());
    m_request->setAccessToken(SoundCloud::accessToken());
    m_request->setRefreshToken(SoundCloud::refreshToken());
        
    connect(m_request, SIGNAL(accessTokenChanged(QString)), SoundCloud::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(refreshTokenChanged(QString)), SoundCloud::instance(), SLOT(setRefreshToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString SoundCloudConnectionModel::errorString() const {
    return m_request->errorString();
}

QSoundCloud::ResourcesRequest::Status SoundCloudConnectionModel::status() const {
    return m_request->status();
}

void SoundCloudConnectionModel::get(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("SoundCloudConnectionModel::get(). Resource path: " + resourcePath, Logger::HighVerbosity);
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_request->get(resourcePath, filters);
    emit statusChanged(status());
}

void SoundCloudConnectionModel::cancel() {
    m_request->cancel();
}

void SoundCloudConnectionModel::reload() {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("SoundCloudConnectionModel::reload(). Resource path: " + m_resourcePath, Logger::HighVerbosity);
    clear();
    m_request->get(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void SoundCloudConnectionModel::onRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        foreach (const QVariant &v, m_request->result().toList()) {
            const QVariantMap connection = v.toMap();
            append(connection.value("display_name").toString(), connection);
        }
    }
    else {
        Logger::log("SoundCloudConnectionModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}
