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

#ifndef SOUNDCLOUDARTIST_H
#define SOUNDCLOUDARTIST_H

#include "artist.h"
#include <qsoundcloud/resourcesrequest.h>

class SoundCloudArtist : public MKArtist
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(bool followed READ isFollowed NOTIFY followedChanged)
    Q_PROPERTY(qint64 followersCount READ followersCount NOTIFY followersCountChanged)
    Q_PROPERTY(bool online READ isOnline NOTIFY onlineChanged)
    Q_PROPERTY(qint64 playlistCount READ playlistCount NOTIFY playlistCountChanged)
    Q_PROPERTY(QSoundCloud::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(qint64 trackCount READ trackCount NOTIFY trackCountChanged)
    Q_PROPERTY(QString websiteTitle READ websiteTitle NOTIFY websiteTitleChanged)
    Q_PROPERTY(QUrl websiteUrl READ websiteUrl NOTIFY websiteUrlChanged)
    
public:
    explicit SoundCloudArtist(QObject *parent = 0);
    explicit SoundCloudArtist(const QString &id, QObject *parent = 0);
    explicit SoundCloudArtist(const QVariantMap &artist, QObject *parent = 0);
    explicit SoundCloudArtist(const SoundCloudArtist *artist, QObject *parent = 0);
        
    QString errorString() const;
    
    bool isFollowed() const;
    qint64 followersCount() const;
    
    bool isOnline() const;
    
    qint64 playlistCount() const;
    
    QSoundCloud::ResourcesRequest::Status status() const;
    
    qint64 trackCount() const;
    
    QString websiteTitle() const;
    QUrl websiteUrl() const;
        
    Q_INVOKABLE void loadArtist(const QString &id);
    Q_INVOKABLE void loadArtist(const QVariantMap &artist);
    Q_INVOKABLE void loadArtist(SoundCloudArtist *artist);

public Q_SLOTS:
    void checkIfFollowed();
    void follow();
    void unfollow();
    void cancel();
        
private Q_SLOTS:
    void onArtistRequestFinished();
    void onFollowCheckRequestFinished();
    void onFollowRequestFinished();
    void onUnfollowRequestFinished();
    void onArtistUpdated(SoundCloudArtist *artist);
    
Q_SIGNALS:
    void followedChanged();
    void followersCountChanged();
    
    void onlineChanged();
    
    void playlistCountChanged();
        
    void statusChanged(QSoundCloud::ResourcesRequest::Status s);
    
    void trackCountChanged();
    
    void websiteTitleChanged();
    void websiteUrlChanged();
    
private:
    void setFollowed(bool s);
    void setFollowersCount(qint64 c);
    
    void setOnline(bool o);
    
    void setPlaylistCount(qint64 c);
    
    void setTrackCount(qint64 c);
    
    void setWebsiteTitle(const QString &t);
    void setWebsiteUrl(const QUrl &u);
    
    void initRequest();
    
    QSoundCloud::ResourcesRequest *m_request;
    
    bool m_followed;
    qint64 m_followersCount;
    
    bool m_online;
    
    qint64 m_playlistCount;
    qint64 m_trackCount;
    
    QString m_websiteTitle;
    QUrl m_websiteUrl;
};
    
#endif // SOUNDCLOUDARTIST_H
