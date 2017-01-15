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

#include "soundcloud.h"
#include "database.h"
#include "logger.h"
#include <qsoundcloud/urls.h>
#include <QSettings>
#include <QSqlRecord>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

const QString SoundCloud::CLIENT_ID("9b7cb759c6d41b14af05855f94bc743c");
const QString SoundCloud::CLIENT_SECRET("f83369b46fe992306a90ef579d57fac4");
const QString SoundCloud::REDIRECT_URI("http://marxoft.co.uk/projects/musikloud2");
const QStringList SoundCloud::SCOPES = QStringList() << QSoundCloud::NON_EXPIRING_SCOPE;

const QRegExp SoundCloud::URL_REGEXP("http(s|)://(api\\.|)soundcloud\\.com/[\\w-_/]+");

SoundCloud::FollowingCache SoundCloud::followingCache;

SoundCloud* SoundCloud::self = 0;

SoundCloud::SoundCloud() :
    QObject()
{
}

SoundCloud::~SoundCloud() {
    self = 0;
}

SoundCloud* SoundCloud::instance() {
    return self ? self : self = new SoundCloud;
}

void SoundCloud::init() {
    if (accessToken().isEmpty()) {
        setUserId(QString());
    }
}

QString SoundCloud::getErrorString(const QVariantMap &error) {    
    if (error.contains("errors")) {
        const QVariantList errors = error.value("errors").toList();
        
        if (!errors.isEmpty()) {
            const QVariantMap em = errors.first().toMap();
        
            if (em.contains("error_message")) {
                return em.value("error_message").toString();
            }
        }
    }
    
    return tr("Unknown error");
}

QUrl SoundCloud::authUrl() {
    QUrl url(QSoundCloud::AUTH_URL);
#if QT_VERSION >= 0x050000
    QUrlQuery query(url);
    query.addQueryItem("client_id", clientId());
    query.addQueryItem("redirect_uri", redirectUri());
    query.addQueryItem("response_type", "code");
    query.addQueryItem("display", "popup");
    
    const QStringList s = scopes();
    
    if (!s.isEmpty()) {
        query.addQueryItem("scope", s.join(" "));
    }
    
    url.setQuery(query);
#else
    url.addQueryItem("client_id", clientId());
    url.addQueryItem("redirect_uri", redirectUri());
    url.addQueryItem("response_type", "code");
    url.addQueryItem("display", "popup");
    
    const QStringList s = scopes();
    
    if (!s.isEmpty()) {
        url.addQueryItem("scope", s.join(" "));
    }
#endif
    return url;
}

QString SoundCloud::userId() {
    return QSettings().value("SoundCloud/userId").toString();
}

void SoundCloud::setUserId(const QString &id) {
    if (id != userId()) {
        QSettings().setValue("SoundCloud/userId", id);
        followingCache.ids.clear();
        followingCache.nextHref = QString();
        followingCache.loaded = false;
        
        if (self) {
            emit self->userIdChanged(id);
        }
    }
}

QString SoundCloud::accessToken() {
    if (userId().isEmpty()) {
        return QString();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT accessToken FROM soundcloudAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("SoundCloud::accessToken: database error: " + query.lastError().text());
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void SoundCloud::setAccessToken(const QString &token) {
    Logger::log("SoundCloud::setAccessToken(). Token: " + token, Logger::MediumVerbosity);
    QSqlQuery query = getDatabase().exec(QString("UPDATE soundcloudAccounts SET accessToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        Logger::log("SoundCloud::setAccessToken: database error: " + query.lastError().text());
    }
    else if (self) {
        emit self->accessTokenChanged(token);
    }
}

QString SoundCloud::refreshToken() {
    if (userId().isEmpty()) {
        return QString();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT refreshToken FROM soundcloudAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("SoundCloud::refreshToken(): database error: " + query.lastError().text());
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void SoundCloud::setRefreshToken(const QString &token) {
    Logger::log("SoundCloud::setRefreshToken(). Token: " + token, Logger::MediumVerbosity);
    QSqlQuery query = getDatabase().exec(QString("UPDATE soundcloudAccounts SET refreshToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        Logger::log("SoundCloud::setRefreshToken(): database error: " + query.lastError().text());
    }
    else if (self) {
        emit self->accessTokenChanged(token);
    }
}

QString SoundCloud::clientId() {
    return QSettings().value("SoundCloud/clientId", CLIENT_ID).toString();
}

void SoundCloud::setClientId(const QString &id) {
    Logger::log("SoundCloud::setClientId(). ID: " + id, Logger::MediumVerbosity);
    
    if (id != clientId()) {
        QSettings().setValue("SoundCloud/clientId", id);
        
        if (self) {
            emit self->clientIdChanged(id);
        }
    }
}

QString SoundCloud::clientSecret() {
    return QSettings().value("SoundCloud/clientSecret", CLIENT_SECRET).toString();
}

void SoundCloud::setClientSecret(const QString &secret) {
    Logger::log("SoundCloud::setClientSecret(). Secret: " + secret, Logger::MediumVerbosity);
    
    if (secret != clientSecret()) {
        QSettings().setValue("SoundCloud/clientSecret", secret);
        
        if (self) {
            emit self->clientSecretChanged(secret);
        }
    }
}

QString SoundCloud::redirectUri() {
    return QSettings().value("SoundCloud/redirectUri", REDIRECT_URI).toString();
}

void SoundCloud::setRedirectUri(const QString &uri) {
    Logger::log("SoundCloud::setRedirectUri(). URI: " + uri, Logger::MediumVerbosity);
    
    if (uri != redirectUri()) {
        QSettings().setValue("SoundCloud/redirectUri", uri);
        
        if (self) {
            emit self->redirectUriChanged(uri);
        }
    }
}

QStringList SoundCloud::scopes() {
    return QSettings().value("SoundCloud/scopes", SCOPES).toStringList();
}

void SoundCloud::setScopes(const QStringList &s) {
    Logger::log("SoundCloud::setScopes(). Scopes: " + s.join(", "), Logger::MediumVerbosity);
    
    if (s != scopes()) {
        QSettings().setValue("SoundCloud/scopes", s);
        
        if (self) {
            emit self->scopesChanged(s);
        }
    }
}

bool SoundCloud::hasScope(const QString &scope) {
    if (userId().isEmpty()) {
        return false;
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT scopes FROM soundcloudAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        Logger::log("SoundCloud::hasScope(): database error: " + query.lastError().text());
        return false;
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString().contains(scope);
    }
    
    return false;
}

QString SoundCloud::nonexpiringScope() {
    return QSoundCloud::NON_EXPIRING_SCOPE;
}

QString SoundCloud::wildcardScope() {
    return QSoundCloud::WILDCARD_SCOPE;
}
