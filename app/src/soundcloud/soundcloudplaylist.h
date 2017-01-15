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

#ifndef SOUNDCLOUDPLAYLIST_H
#define SOUNDCLOUDPLAYLIST_H

#include "playlist.h"
#include <qsoundcloud/resourcesrequest.h>

class SoundCloudPlaylist : public MKPlaylist
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QString sharing READ sharing NOTIFY sharingChanged)
    Q_PROPERTY(QSoundCloud::ResourcesRequest::Status status READ status NOTIFY statusChanged)

public:
    explicit SoundCloudPlaylist(QObject *parent = 0);
    explicit SoundCloudPlaylist(const QString &id, QObject *parent = 0);
    explicit SoundCloudPlaylist(const QVariantMap &playlist, QObject *parent = 0);
    explicit SoundCloudPlaylist(const SoundCloudPlaylist *playlist, QObject *parent = 0);
    
    QString errorString() const;
        
    QString sharing() const;
        
    QSoundCloud::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadPlaylist(const QString &id);
    Q_INVOKABLE void loadPlaylist(const QVariantMap &playlist);
    Q_INVOKABLE void loadPlaylist(SoundCloudPlaylist *playlist);

public Q_SLOTS:
    void cancel();

private Q_SLOTS:
    void onPlaylistRequestFinished();
    
Q_SIGNALS:
    void sharingChanged();
    
    void statusChanged(QSoundCloud::ResourcesRequest::Status s);

private:
    void setSharing(const QString &s);
    
    void initRequest();
    
    QSoundCloud::ResourcesRequest *m_request;
        
    QString m_sharing;
};

#endif // SOUNDCLOUDPLAYLIST_H
