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

#ifndef SOUNDCLOUDTRACK_H
#define SOUNDCLOUDTRACK_H

#include "track.h"
#include <qsoundcloud/resourcesrequest.h>

class SoundCloudTrack : public MKTrack
{
    Q_OBJECT
    
    Q_PROPERTY(bool commentable READ isCommentable NOTIFY commentableChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(bool favourited READ isFavourite NOTIFY favouriteChanged)
    Q_PROPERTY(qint64 favouriteCount READ favouriteCount NOTIFY favouriteCountChanged)
    Q_PROPERTY(QString sharing READ sharing NOTIFY sharingChanged)
    Q_PROPERTY(QSoundCloud::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QUrl waveformUrl READ waveformUrl NOTIFY waveformUrlChanged)

public:
    explicit SoundCloudTrack(QObject *parent = 0);
    explicit SoundCloudTrack(const QString &id, QObject *parent = 0);
    explicit SoundCloudTrack(const QVariantMap &track, QObject *parent = 0);
    explicit SoundCloudTrack(const SoundCloudTrack *track, QObject *parent = 0);
    
    bool isCommentable() const;
    
    QString errorString() const;
    
    bool isFavourite() const;
    qint64 favouriteCount() const;
    
    QString sharing() const;
            
    QSoundCloud::ResourcesRequest::Status status() const;
        
    QUrl waveformUrl() const;
    
    Q_INVOKABLE void loadTrack(const QString &id);
    Q_INVOKABLE void loadTrack(const QVariantMap &track);
    Q_INVOKABLE void loadTrack(SoundCloudTrack *track);
    
public Q_SLOTS:
    void favourite();
    void unfavourite();
    void cancel();
        
private Q_SLOTS:
    void onTrackRequestFinished();
    void onFavouriteRequestFinished();
    void onUnfavouriteRequestFinished();
    void onTrackUpdated(SoundCloudTrack *track);
    
Q_SIGNALS:
    void commentableChanged();
    
    void favouriteChanged();
    void favouriteCountChanged();
    
    void sharingChanged();
    
    void statusChanged(QSoundCloud::ResourcesRequest::Status s);
        
    void waveformUrlChanged();

private:
    void initRequest();
    
    void setCommentable(bool c);
    
    void setFavourite(bool f);
    void setFavouriteCount(qint64 c);
    
    void setSharing(const QString &s);
        
    void setWaveformUrl(const QUrl &u);
    
    QSoundCloud::ResourcesRequest *m_request;
    
    bool m_commentable;
    
    bool m_favourite;
    qint64 m_favouriteCount;
    
    QString m_sharing;
        
    QUrl m_waveformUrl;
};

#endif // SOUNDCLOUDTRACK_H
