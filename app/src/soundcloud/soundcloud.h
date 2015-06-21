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

#ifndef SOUNDCLOUD_H
#define SOUNDCLOUD_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include <QUrl>

class SoundCloudArtist;
class SoundCloudComment;
class SoundCloudPlaylist;
class SoundCloudTrack;

class SoundCloud : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)
    Q_PROPERTY(QString refreshToken READ refreshToken WRITE setRefreshToken NOTIFY refreshTokenChanged)
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)
    Q_PROPERTY(QString clientSecret READ clientSecret WRITE setClientSecret NOTIFY clientSecretChanged)
    Q_PROPERTY(QString redirectUri READ redirectUri WRITE setRedirectUri NOTIFY redirectUriChanged)
    Q_PROPERTY(QStringList scopes READ scopes WRITE setScopes NOTIFY scopesChanged)
    Q_PROPERTY(QString NON_EXPIRING_SCOPE READ nonexpiringScope CONSTANT)
    Q_PROPERTY(QString WILDCARD_SCOPE READ wildcardScope CONSTANT)
        
public:
    explicit SoundCloud(QObject *parent = 0);
    ~SoundCloud();
        
    static SoundCloud* instance();
            
    Q_INVOKABLE static QString getErrorString(const QVariantMap &error);
    
    Q_INVOKABLE QUrl authUrl() const;
    
    QString userId() const;
    
    QString accessToken() const;
    
    QString refreshToken() const;
            
    QString clientId() const;
    
    QString clientSecret() const;
    
    QString redirectUri() const;
    
    QStringList scopes() const;
    
    Q_INVOKABLE bool hasScope(const QString &scope) const;
    
    static QString nonexpiringScope();
    static QString wildcardScope();
    
public Q_SLOTS:    
    void setUserId(const QString &id);
    
    void setAccessToken(const QString &token);
    
    void setRefreshToken(const QString &token);
    
    void setClientId(const QString &id);
    
    void setClientSecret(const QString &secret);
    
    void setRedirectUri(const QString &uri);
    
    void setScopes(const QStringList &s);
    
Q_SIGNALS:
    void userIdChanged();
    void accessTokenChanged();
    void refreshTokenChanged();
    void clientIdChanged();
    void clientSecretChanged();
    void redirectUriChanged();
    void scopesChanged();
    
    void artistFollowed(SoundCloudArtist *artist);
    void artistUnfollowed(SoundCloudArtist *artist);

    void commentAdded(SoundCloudComment *comment);
    
    void trackFavourited(SoundCloudTrack *track);
    void trackUnfavourited(SoundCloudTrack *track);    

private:
    struct FollowingCache {
        QStringList ids;
        QString nextHref;
        QVariantMap filters;
        bool loaded;

        FollowingCache() :
            loaded(false)
        {
            filters["limit"] = 200;
            filters["linked_partitioning"] = true;
        }
    };
    
    static FollowingCache followingCache;
    static SoundCloud *self;
    
    friend class SoundCloudArtist;
    friend class SoundCloudComment;
    friend class SoundCloudPlaylist;
    friend class SoundCloudTrack;  
};

#endif // SOUNDCLOUD_H
