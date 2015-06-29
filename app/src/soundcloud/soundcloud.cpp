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
#include <qsoundcloud/urls.h>
#include <QSettings>
#include <QSqlRecord>
#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif
#ifdef MUSIKLOUD_DEBUG
#include <QDebug>
#endif

static const QString CLIENT_ID("9b7cb759c6d41b14af05855f94bc743c");
static const QString CLIENT_SECRET("f83369b46fe992306a90ef579d57fac4");
static const QString REDIRECT_URI("http://marxoft.co.uk/projects/musikloud2");
static const QStringList SCOPES = QStringList() << QSoundCloud::NON_EXPIRING_SCOPE;

SoundCloud::FollowingCache SoundCloud::followingCache;

SoundCloud* SoundCloud::self = 0;

SoundCloud::SoundCloud(QObject *parent) :
    QObject(parent)
{
    if (!self) {
        self = this;
    }
}

SoundCloud::~SoundCloud() {
    if (self == this) {
        self = 0;
    }
}

SoundCloud* SoundCloud::instance() {
    return self;
}

QString SoundCloud::getErrorString(const QVariantMap &error) {    
    if (error.contains("errors")) {
        QVariantList errors = error.value("errors").toList();
        
        if (!errors.isEmpty()) {
            QVariantMap em = errors.first().toMap();
        
            if (em.contains("error_message")) {
                return em.value("error_message").toString();
            }
        }
    }
    
    return tr("Unknown error");
}

QUrl SoundCloud::authUrl() const {
    QUrl url(QSoundCloud::AUTH_URL);
#if QT_VERSION >= 0x050000
    QUrlQuery query(url);
    query.addQueryItem("client_id", clientId());
    query.addQueryItem("redirect_uri", redirectUri());
    query.addQueryItem("response_type", "code");
    query.addQueryItem("display", "popup");
    
    QStringList s = scopes();
    
    if (!s.isEmpty()) {
        query.addQueryItem("scope", s.join(" "));
    }
    
    url.setQuery(query);
#else
    url.addQueryItem("client_id", clientId());
    url.addQueryItem("redirect_uri", redirectUri());
    url.addQueryItem("response_type", "code");
    url.addQueryItem("display", "popup");
    
    QStringList s = scopes();
    
    if (!s.isEmpty()) {
        url.addQueryItem("scope", s.join(" "));
    }
#endif
    return url;
}

QString SoundCloud::userId() const {
    return QSettings().value("SoundCloud/userId").toString();
}

void SoundCloud::setUserId(const QString &id) {
    if (id != userId()) {
        QSettings().setValue("SoundCloud/userId", id);
        followingCache.ids.clear();
        followingCache.nextHref = QString();
        followingCache.loaded = false;
        emit userIdChanged();
    }
}

QString SoundCloud::accessToken() const {
    if (userId().isEmpty()) {
        return QString();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT accessToken FROM soundcloudAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        qDebug() << "SoundCloud::accessToken: database error:" << query.lastError().text();
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void SoundCloud::setAccessToken(const QString &token) {
    QSqlQuery query = getDatabase().exec(QString("UPDATE soundcloudAccounts SET accessToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        qDebug() << "SoundCloud::setAccessToken: database error:" << query.lastError().text();
    }
    else {
        emit accessTokenChanged();
    }
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "SoundCloud::setAccessToken" << token;
#endif
}

QString SoundCloud::refreshToken() const {
    if (userId().isEmpty()) {
        return QString();
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT refreshToken FROM soundcloudAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        qDebug() << "SoundCloud::refreshToken: database error:" << query.lastError().text();
        return QString();
    }
    
    if ((query.record().count() > 0) && (query.next())) {
         return query.value(0).toString();
    }
    
    return QString();
}

void SoundCloud::setRefreshToken(const QString &token) {
    QSqlQuery query = getDatabase().exec(QString("UPDATE soundcloudAccounts SET refreshToken = '%1' WHERE userId = '%2'")
                                                .arg(token).arg(userId()));

    if (query.lastError().isValid()) {
        qDebug() << "SoundCloud::setRefreshToken: database error:" << query.lastError().text();
    }
    else {
        emit accessTokenChanged();
    }
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "SoundCloud::setRefreshToken" << token;
#endif
}

QString SoundCloud::clientId() const {
    return QSettings().value("SoundCloud/clientId", CLIENT_ID).toString();
}

void SoundCloud::setClientId(const QString &id) {
    if (id != clientId()) {
        QSettings().setValue("SoundCloud/clientId", id);
        emit clientIdChanged();
    }
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "SoundCloud::setClientId" << id;
#endif
}

QString SoundCloud::clientSecret() const {
    return QSettings().value("SoundCloud/clientSecret", CLIENT_SECRET).toString();
}

void SoundCloud::setClientSecret(const QString &secret) {
    if (secret != clientSecret()) {
        QSettings().setValue("SoundCloud/clientSecret", secret);
        emit clientSecretChanged();
    }
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "SoundCloud::setClientSecret" << secret;
#endif
}

QString SoundCloud::redirectUri() const {
    return QSettings().value("SoundCloud/redirectUri", REDIRECT_URI).toString();
}

void SoundCloud::setRedirectUri(const QString &uri) {
    if (uri != redirectUri()) {
        QSettings().setValue("SoundCloud/redirectUri", uri);
        emit redirectUriChanged();
    }
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "SoundCloud::setRedirectUri" << uri;
#endif
}

QStringList SoundCloud::scopes() const {
    return QSettings().value("SoundCloud/scopes", SCOPES).toStringList();
}

void SoundCloud::setScopes(const QStringList &s) {
    if (s != scopes()) {
        QSettings().setValue("SoundCloud/scopes", s);
        emit scopesChanged();
    }
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "SoundCloud::setScopes" << s;
#endif
}

bool SoundCloud::hasScope(const QString &scope) const {
    if (userId().isEmpty()) {
        return false;
    }
    
    QSqlQuery query = getDatabase().exec(QString("SELECT scopes FROM soundcloudAccounts WHERE userId = '%1'")
                                                .arg(userId()));
    
    if (query.lastError().isValid()) {
        qDebug() << "SoundCloud::hasScope: database error:" << query.lastError().text();
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
