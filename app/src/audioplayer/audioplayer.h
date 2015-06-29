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

#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "pluginstreammodel.h"
#include "soundcloudstreammodel.h"
#include "trackmodel.h"
#include <QMediaPlayer>

class AudioPlayer : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(int bufferStatus READ bufferStatus NOTIFY bufferStatusChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(MKTrack* currentTrack READ currentTrack NOTIFY currentIndexChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString durationString READ durationString NOTIFY durationChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY statusChanged)
    Q_PROPERTY(bool playing READ isPlaying WRITE setPlaying NOTIFY statusChanged)
    Q_PROPERTY(bool stopped READ isStopped NOTIFY statusChanged)
    Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString positionString READ positionString NOTIFY positionChanged)
    Q_PROPERTY(TrackModel* queue READ queue CONSTANT)
    Q_PROPERTY(int queueCount READ queueCount NOTIFY queueCountChanged)
    Q_PROPERTY(bool repeat READ repeatEnabled WRITE setRepeatEnabled NOTIFY repeatEnabledChanged)
    Q_PROPERTY(bool seekable READ isSeekable NOTIFY seekableChanged)
    Q_PROPERTY(bool shuffle READ shuffleEnabled WRITE setShuffleEnabled NOTIFY shuffleEnabledChanged)
    Q_PROPERTY(bool stopAfterCurrentTrack READ stopAfterCurrentTrack WRITE setStopAfterCurrentTrack
               NOTIFY stopAfterCurrentTrackChanged) 
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    
    Q_ENUMS(Status)

public:
    enum Status {
        Stopped = 0,
        Paused,
        Loading,
        Loaded,
        Buffering,
        Buffered,
        Playing,
        Failed
    };
    
    explicit AudioPlayer(QObject *parent = 0);
    ~AudioPlayer();
    
    static AudioPlayer* instance();
    
    int bufferStatus() const;
        
    int currentIndex() const;
    
    MKTrack* currentTrack() const;
        
    qint64 duration() const;
    QString durationString() const;
    
    QString errorString() const;
    
    bool isPaused() const;
    bool isPlaying() const;
    bool isStopped() const;
    
    qint64 position() const;
    QString positionString() const;
    
    TrackModel* queue() const;
        
    int queueCount() const;
    
    bool repeatEnabled() const;
    
    bool isSeekable() const;
    
    bool shuffleEnabled() const;
    
    bool stopAfterCurrentTrack() const;
    
    Status status() const;
    
public Q_SLOTS:
    void setCurrentIndex(int i);
    
    void setPaused(bool p);
    void setPlaying(bool p);
    
    void setPosition(qint64 p);
    
    void setRepeatEnabled(bool r);
    
    void setShuffleEnabled(bool s);
    
    void setStopAfterCurrentTrack(bool s);
    
    bool addFolder(const QString &folder);
        
    void addTrack(MKTrack *track);
    void addTracks(const QList<MKTrack*> &tracks);
    void removeTrack(int i);    
    
    void addUrl(const QUrl &url);
    void addUrls(const QList<QUrl> &urls);
    
    void clearQueue();
    
    void next();
    void pause();
    void play();
    bool playFolder(const QString &folder);
    void playTrack(MKTrack *track);
    void playTracks(const QList<MKTrack*> &tracks);
    void playUrl(const QUrl &url);
    void playUrls(const QList<QUrl> &urls);
    void previous();
    void stop();
    
private:
    void setErrorString(const QString &e);
    
    void setStatus(Status s);
    
    void initPluginModel();
    void initSoundCloudModel();

private Q_SLOTS:
    void shuffleTracks();
    
    void onBufferStatusChanged(int b);
    void onError(QMediaPlayer::Error e);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus m);
    void onPluginModelStatusChanged(ResourcesRequest::Status s);
    void onSoundCloudModelStatusChanged(QSoundCloud::StreamsRequest::Status s);
    void onStateChanged(QMediaPlayer::State s);

Q_SIGNALS:
    void bufferStatusChanged(int b);
    
    void currentIndexChanged(int i);
    
    void durationChanged(qint64 d);
    
    void positionChanged(qint64 p);
    
    void queueCountChanged(int c);
    
    void repeatEnabledChanged(bool r);
    
    void seekableChanged(bool s);
    
    void shuffleEnabledChanged(bool s);
    
    void stopAfterCurrentTrackChanged(bool s);
    
    void statusChanged(AudioPlayer::Status s);

private:
    static AudioPlayer *self;
    
    QMediaPlayer *m_player;
    TrackModel *m_queue;
    
    SoundCloudStreamModel *m_soundcloudModel;
    PluginStreamModel *m_pluginModel;
    
    int m_index;
    int m_shuffleIndex;
        
    QString m_errorString;
    
    bool m_repeat;
    
    bool m_shuffle;
    QList<int> m_shuffleOrder;
    
    bool m_stopAfterCurrentTrack;
    
    Status m_status;
};
    
#endif // AUDIOPLAYER_H
