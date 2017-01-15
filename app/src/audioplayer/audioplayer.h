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
#include <QTimer>

class AudioPlayerMetaData : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString artist READ artist NOTIFY changed)
    Q_PROPERTY(QString artistId READ artistId NOTIFY changed)
    Q_PROPERTY(bool available READ isAvailable NOTIFY changed)
    Q_PROPERTY(QString date READ date NOTIFY changed)
    Q_PROPERTY(QString description READ description NOTIFY changed)
    Q_PROPERTY(bool downloadable READ isDownloadable NOTIFY changed)
    Q_PROPERTY(qint64 duration READ duration NOTIFY changed)
    Q_PROPERTY(QString durationString READ durationString NOTIFY changed)
    Q_PROPERTY(QString format READ format NOTIFY changed)
    Q_PROPERTY(QString genre READ genre NOTIFY changed)
    Q_PROPERTY(QString id READ id NOTIFY changed)
    Q_PROPERTY(QUrl largeThumbnailUrl READ largeThumbnailUrl NOTIFY changed)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY changed)
    Q_PROPERTY(QString service READ service NOTIFY changed)
    Q_PROPERTY(qint64 size READ size NOTIFY changed)
    Q_PROPERTY(QString sizeString READ sizeString NOTIFY changed)
    Q_PROPERTY(QUrl streamUrl READ streamUrl NOTIFY changed)
    Q_PROPERTY(QString title READ title NOTIFY changed)
    Q_PROPERTY(QUrl url READ url NOTIFY changed)
    
    Q_ENUMS(MetaData)

public:
    enum MetaData {
        Artist = 0,
        ArtistId,
        Date,
        Description,
        Downloadable,
        Duration,
        DurationString,
        Format,
        Genre,
        Id,
        LargeThumbnailUrl,
        ThumbnailUrl,
        Service,
        Size,
        SizeString,
        StreamUrl,
        Title,
        Url
    };
    
    explicit AudioPlayerMetaData(QObject *parent = 0);
        
    static QString artist();
    static QString artistId();

    static QString date();

    static QString description();
    
    static qint64 duration();
    static QString durationString();

    static QString format();
    
    static QString genre();

    static QString id();
    
    static bool isAvailable();

    static bool isDownloadable();
    
    static QUrl largeThumbnailUrl();
    static QUrl thumbnailUrl();

    static QString service();

    static qint64 size();
    static QString sizeString();

    static QUrl streamUrl();
    
    static QString title();
    
    static QUrl url();

Q_SIGNALS:
    void changed();
};

class AudioPlayer : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(AudioPlayerMetaData* metaData READ metaData CONSTANT)
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
    Q_PROPERTY(int sleepTimerDuration READ sleepTimerDuration WRITE setSleepTimerDuration
               NOTIFY sleepTimerDurationChanged)
    Q_PROPERTY(QString sleepTimerDurationString READ sleepTimerDurationString NOTIFY sleepTimerDurationChanged)
    Q_PROPERTY(bool sleepTimerEnabled READ sleepTimerEnabled WRITE setSleepTimerEnabled
               NOTIFY sleepTimerEnabledChanged)
    Q_PROPERTY(int sleepTimerRemaining READ sleepTimerRemaining NOTIFY sleepTimerRemainingChanged)
    Q_PROPERTY(QString sleepTimerRemainingString READ sleepTimerRemainingString NOTIFY sleepTimerRemainingChanged)
    Q_PROPERTY(bool stopAfterCurrentTrack READ stopAfterCurrentTrack WRITE setStopAfterCurrentTrack
               NOTIFY stopAfterCurrentTrackChanged) 
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    
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
    
    AudioPlayerMetaData* metaData();
    QVariant metaData(AudioPlayerMetaData::MetaData key) const;
    
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
    
    int sleepTimerDuration() const;
    QString sleepTimerDurationString() const;
    bool sleepTimerEnabled() const;
    int sleepTimerRemaining() const;
    QString sleepTimerRemainingString() const;
    
    bool stopAfterCurrentTrack() const;
    
    Status status() const;
    
    int volume() const;
    
public Q_SLOTS:
    void setCurrentIndex(int i, bool autoPlay = false);
    
    void setPaused(bool p);
    void setPlaying(bool p);
    void togglePlaying();
    
    void setPosition(qint64 p);
    
    void setRepeatEnabled(bool r);
    
    void setShuffleEnabled(bool s);
    
    void setSleepTimerDuration(int d);
    void setSleepTimerEnabled(bool e);
    
    void setStopAfterCurrentTrack(bool s);
    
    void setVolume(int v);
            
    void addTrack(MKTrack *track);
    void addTracks(const QList<MKTrack*> &tracks);
    void addTracks(const QVariantList &tracks); // For QML
    void removeTrack(int i);    
    
    int addUrl(const QUrl &url);
    int addUrls(const QList<QUrl> &urls);
    
    void clearQueue();
    int restoreQueue();
    void saveQueue();
    
    void next(bool autoPlay = false);
    void pause();
    void play();
    void playTrack(MKTrack *track);
    void playTracks(const QList<MKTrack*> &tracks);
    void playTracks(const QVariantList &tracks); // For QML
    int playUrl(const QUrl &url);
    int playUrls(const QList<QUrl> &urls);
    void previous(bool autoPlay = false);
    void stop();    

private Q_SLOTS:
    void shuffleTracks();
    
    void onBufferStatusChanged(int b);
    void onDurationChanged(qint64 d);
    void onError(QMediaPlayer::Error e);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus m);
    void onMetaDataChanged();
    void onPluginModelStatusChanged(ResourcesRequest::Status s);
    void onSeekableChanged();
    void onSleepTimerTimeout();
    void onSoundCloudModelStatusChanged(QSoundCloud::StreamsRequest::Status s);
    void onStateChanged(QMediaPlayer::State s);

Q_SIGNALS:
    void bufferStatusChanged(int b);
    
    void currentIndexChanged(int i);
    
    void durationChanged(qint64 d);

    void error(const QString &e);
    
    void metaDataChanged();
    
    void positionChanged(qint64 p);
    
    void queueCountChanged(int c);
    
    void repeatEnabledChanged(bool r);
    
    void seekableChanged(bool s);
    
    void shuffleEnabledChanged(bool s);
    
    void sleepTimerDurationChanged(int d);
    void sleepTimerEnabledChanged(bool e);
    void sleepTimerRemainingChanged(int r);
    
    void stopAfterCurrentTrackChanged(bool s);
    
    void statusChanged(AudioPlayer::Status s);
    
    void volumeChanged(int v);

private:
    void setErrorString(const QString &e);
    
    void setSleepTimerRemaining(int r);
    
    void setStatus(Status s);
    
    void addLocalFile(const QString &fileName);
    
    MKTrack* trackFromUrl(const QUrl &url);
    
    void initPluginModel();
    void initSoundCloudModel();
    
    static AudioPlayer *self;
    
    QMediaPlayer *m_player;
    AudioPlayerMetaData *m_metaData;
    TrackModel *m_queue;
    
    SoundCloudStreamModel *m_soundcloudModel;
    PluginStreamModel *m_pluginModel;
    
    int m_index;
    int m_shuffleIndex;
        
    QString m_errorString;
    
    bool m_repeat;
    
    bool m_shuffle;
    QList<int> m_shuffleOrder;
    
    QTimer m_sleepTimer;
    int m_sleepTimerDuration;
    int m_sleepTimerRemaining;
    
    bool m_stopAfterCurrentTrack;
    
    Status m_status;
    
    bool m_metaDataSet;
};

#endif // AUDIOPLAYER_H
