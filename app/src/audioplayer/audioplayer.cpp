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

#include "audioplayer.h"
#include "definitions.h"
#include "localtrack.h"
#include "resources.h"
#include "settings.h"
#include "utils.h"
#include <QDir>
#include <algorithm>
#ifdef MUSIKLOUD_DEBUG
#include <QDebug>
#endif

AudioPlayer* AudioPlayer::self = 0;

AudioPlayer::AudioPlayer(QObject *parent) :
    QObject(parent),
    m_player(new QMediaPlayer(this)),
    m_queue(new TrackModel(this)),
    m_soundcloudModel(0),
    m_pluginModel(0),
    m_index(0),
    m_shuffleIndex(0),
    m_repeat(false),
    m_shuffle(false),
    m_stopAfterCurrentTrack(false),
    m_status(Stopped)
{
    if (!self) {
        self = this;
    }
    
    connect(m_player, SIGNAL(bufferStatusChanged(int)), this, SLOT(onBufferStatusChanged(int)));
    connect(m_player, SIGNAL(durationChanged(qint64)), this, SIGNAL(durationChanged(qint64)));
    connect(m_player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onError(QMediaPlayer::Error)));
    connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_player, SIGNAL(positionChanged(qint64)), this, SIGNAL(positionChanged(qint64)));
    connect(m_player, SIGNAL(seekableChanged(bool)), this, SIGNAL(seekableChanged(bool)));
    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(onStateChanged(QMediaPlayer::State)));
    connect(m_queue, SIGNAL(countChanged(int)), this, SIGNAL(queueCountChanged(int)));
}

AudioPlayer::~AudioPlayer() {
    if (self == this) {
        self = 0;
    }
}

AudioPlayer* AudioPlayer::instance() {
    return self;
}

int AudioPlayer::bufferStatus() const {
    return m_player->bufferStatus();
}

int AudioPlayer::currentIndex() const {
    return m_index;
}

void AudioPlayer::setCurrentIndex(int i) {
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "AudioPlayer::setCurrentIndex" << i;
#endif
    if ((i >= 0) && (i < queueCount())) {
        m_index = i;
        emit currentIndexChanged(i);
        
        if (MKTrack *track = currentTrack()) {
            stop();
            
            if (!track->streamUrl().isEmpty()) {
                m_player->setMedia(track->streamUrl());
                m_player->play();
            }
            else if (track->service() == Resources::SOUNDCLOUD) {
                initSoundCloudModel();
                m_soundcloudModel->get(track->id());
            }
            else {
                initPluginModel();
                m_pluginModel->setService(track->service());
                m_pluginModel->list(track->id());
            }
        }
    }
}

MKTrack* AudioPlayer::currentTrack() const {
    return m_queue->get(currentIndex());
}

qint64 AudioPlayer::duration() const {
    return m_player->duration();
}

QString AudioPlayer::durationString() const {
    return Utils::formatMSecs(duration());
}

QString AudioPlayer::errorString() const {
    return m_errorString;
}

void AudioPlayer::setErrorString(const QString &e) {
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "AudioPlayer::setErrorString" << e;
#endif
    m_errorString = e;
}

bool AudioPlayer::isPaused() const {
    return status() == Paused;
}

void AudioPlayer::setPaused(bool p) {
    if (p) {
        pause();
    }
    else if (isPaused()) {
        play();
    }
}

bool AudioPlayer::isPlaying() const {
    return status() == Playing;
}

void AudioPlayer::setPlaying(bool p) {
    if (p) {
        play();
    }
    else if (isPlaying()) {
        pause();
    }
}

bool AudioPlayer::isStopped() const {
    return status() == Stopped;
}

qint64 AudioPlayer::position() const {
    return m_player->position();
}

void AudioPlayer::setPosition(qint64 p) {
    m_player->setPosition(p);
}

QString AudioPlayer::positionString() const {
    return Utils::formatMSecs(position());
}

TrackModel* AudioPlayer::queue() const {
    return m_queue;
}

int AudioPlayer::queueCount() const {
    return m_queue->rowCount();
}

bool AudioPlayer::isSeekable() const {
    return m_player->isSeekable();
}

bool AudioPlayer::repeatEnabled() const {
    return m_repeat;
}

void AudioPlayer::setRepeatEnabled(bool r) {
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "AudioPlayer::setRepeatEnabled" << r;
#endif
    if (r != repeatEnabled()) {
        m_repeat = r;
        emit repeatEnabledChanged(r);
    }
}

bool AudioPlayer::shuffleEnabled() const {
    return m_shuffle;
}

void AudioPlayer::setShuffleEnabled(bool s) {
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "AudioPlayer::setShuffleEnabled" << s;
#endif
    if (s != shuffleEnabled()) {
        m_shuffle = s;
        emit shuffleEnabledChanged(s);
        
        if (s) {
            shuffleTracks();
        }
    }
}

bool AudioPlayer::stopAfterCurrentTrack() const {
    return m_stopAfterCurrentTrack;
}

void AudioPlayer::setStopAfterCurrentTrack(bool s) {
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "AudioPlayer::setStopAfterCurrentTrack" << s;
#endif
    if (s != stopAfterCurrentTrack()) {
        m_stopAfterCurrentTrack = s;
        emit stopAfterCurrentTrackChanged(s);
    }
}

AudioPlayer::Status AudioPlayer::status() const {
    return m_status;
}

void AudioPlayer::setStatus(AudioPlayer::Status s) {
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "AudioPlayer::setStatus" << s;
#endif
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

bool AudioPlayer::addFolder(const QString &folder) {
    QList<QUrl> urls;
    QDir dir(folder);
    
    foreach (QString fileName, dir.entryList(SUPPORTED_AUDIO_FORMATS, QDir::Files)) {
        urls << QUrl::fromLocalFile(dir.absoluteFilePath(fileName));
    }
    
    addUrls(urls);
    return !urls.isEmpty();
}

void AudioPlayer::addTrack(MKTrack *track) {
    m_queue->append(new MKTrack(track, m_queue));
    
    if (shuffleEnabled()) {
        shuffleTracks();
    }
}

void AudioPlayer::addTracks(const QList<MKTrack*> &tracks) {
    foreach (MKTrack *track, tracks) {
        m_queue->append(new MKTrack(track, m_queue));
    }
    
    if (shuffleEnabled()) {
        shuffleTracks();
    }
}

void AudioPlayer::removeTrack(int i) {
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "AudioPlayer::removeTrack" << i;
#endif
    const int current = currentIndex();
    m_queue->remove(i);
    
    if (!m_shuffleOrder.isEmpty()) {
        m_shuffleOrder.removeOne(i);
    
        if (m_shuffleOrder.indexOf(i) <= m_shuffleIndex) {
            m_shuffleIndex--;
        }
    
        for (int j = i; j < m_shuffleOrder.size(); j++) {
            m_shuffleOrder[j]--;
        }
    }
    
    if (i <= current) {
        m_index--;
        
        if (i == current) {
            if (shuffleEnabled()) {
                if (m_shuffleIndex < (m_shuffleOrder.size() - 1)) {
                    next();
                }
                else {
                    stop();
                }
            }
            else if (i < queueCount()) {
                next();
            }
            else {
                stop();
            }
        }
        else {
            emit currentIndexChanged(m_index);
        }
    }
}

void AudioPlayer::addUrl(const QUrl &url) {
    m_queue->append(new LocalTrack(url, m_queue));
    
    if (shuffleEnabled()) {
        shuffleTracks();
    }
}

void AudioPlayer::addUrls(const QList<QUrl> &urls) {
    foreach (QUrl url, urls) {
        m_queue->append(new LocalTrack(url, m_queue));
    }
    
    if (shuffleEnabled()) {
        shuffleTracks();
    }
}

void AudioPlayer::clearQueue() {
    stop();
    m_queue->clear();
    m_shuffleOrder.clear();
    m_index = 0;
    m_shuffleIndex = 0;
}

void AudioPlayer::next() {
    if (shuffleEnabled()) {
        if (m_shuffleIndex < (m_shuffleOrder.size() - 1)) {
            m_shuffleIndex++;
            setCurrentIndex(m_shuffleOrder.at(m_shuffleIndex));
        }
    }
    else {
        setCurrentIndex(currentIndex() + 1);
    }
}

void AudioPlayer::pause() {
    m_player->pause();
}

void AudioPlayer::play() {
    if (status() == Paused) {
        m_player->play();
    }
    else {
        setCurrentIndex(currentIndex());
    }
}

bool AudioPlayer::playFolder(const QString &folder) {
    QList<QUrl> urls;
    QDir dir(folder);
    
    foreach (QString fileName, dir.entryList(SUPPORTED_AUDIO_FORMATS, QDir::Files)) {
        urls << QUrl::fromLocalFile(dir.absoluteFilePath(fileName));
    }
    
    if (urls.isEmpty()) {
        return false;
    }
    
    playUrls(urls);
    return true;
}

void AudioPlayer::playTrack(MKTrack *track) {
    clearQueue();
    addTrack(track);
    play();
}

void AudioPlayer::playTracks(const QList<MKTrack*> &tracks) {
    clearQueue();
    addTracks(tracks);
    play();
}

void AudioPlayer::playUrl(const QUrl &url) {
    clearQueue();
    addUrl(url);
    play();
}

void AudioPlayer::playUrls(const QList<QUrl> &urls) {
    clearQueue();
    addUrls(urls);
    play();
}

void AudioPlayer::previous() {
    if (shuffleEnabled()) {
        if (m_shuffleIndex > 0) {
            m_shuffleIndex--;
            setCurrentIndex(m_shuffleOrder.at(m_shuffleIndex));
        }
    }
    else {
        setCurrentIndex(currentIndex() - 1);
    }
}

void AudioPlayer::stop() {
    m_player->stop();
    
    if (m_soundcloudModel) {
        m_soundcloudModel->cancel();
    }
    
    if (m_pluginModel) {
        m_pluginModel->cancel();
    }
}

void AudioPlayer::initPluginModel() {
    if (!m_pluginModel) {
        m_pluginModel = new PluginStreamModel(this);
        connect(m_pluginModel, SIGNAL(statusChanged(ResourcesRequest::Status)),
                this, SLOT(onPluginModelStatusChanged(ResourcesRequest::Status)));
    }
}

void AudioPlayer::initSoundCloudModel() {
    if (!m_soundcloudModel) {
        m_soundcloudModel = new SoundCloudStreamModel(this);
        connect(m_soundcloudModel, SIGNAL(statusChanged(QSoundCloud::StreamsRequest::Status)),
                this, SLOT(onSoundCloudModelStatusChanged(QSoundCloud::StreamsRequest::Status)));
    }
}

void AudioPlayer::shuffleTracks() {
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "AudioPlayer::shuffleTracks before:" << m_shuffleOrder;
#endif
    const int oldSize = m_shuffleOrder.size();
    
    for (int i = oldSize; i < queueCount(); i++) {
        m_shuffleOrder << i;
    }
    
    if (m_shuffleOrder.size() > oldSize) {
        std::random_shuffle(m_shuffleOrder.begin() + oldSize, m_shuffleOrder.end());
    }
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "AudioPlayer::shuffleTracks after:" << m_shuffleOrder;
#endif
}

void AudioPlayer::onBufferStatusChanged(int b) {
    // Workaround as QMediaPlayer does not notify when mediaStatus changes to QMediaPlayer::BufferingMedia
    if (b < 100) {
        setStatus(Buffering);
    }
    
    emit bufferStatusChanged(b);
}

void AudioPlayer::onError(QMediaPlayer::Error e) {
    if (e != QMediaPlayer::NoError) {
        setErrorString(m_player->errorString());
        setStatus(Failed);
    }
}

void AudioPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus m) {
    switch (m) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
        setStatus(Stopped);
        break;
    case QMediaPlayer::LoadingMedia:
        setStatus(Loading);
        break;
    case QMediaPlayer::StalledMedia:
    case QMediaPlayer::BufferingMedia:
        setStatus(Buffering);
        break;
    case QMediaPlayer::BufferedMedia:
        setStatus(Playing);
        break;
    case QMediaPlayer::EndOfMedia:
        if (!stopAfterCurrentTrack()) {
            if (repeatEnabled()) {
                play();
            }
            else {
                next();
            }
        }
        
        break;
    default:
        break;
    }
}

void AudioPlayer::onPluginModelStatusChanged(ResourcesRequest::Status s) {
    switch (s) {
    case ResourcesRequest::Loading:
        setStatus(Loading);
        break;
    case ResourcesRequest::Ready:
        setStatus(Loaded);
        
        if (m_pluginModel->rowCount() > 0) {
            m_player->setMedia(QUrl(m_pluginModel->data(qMax(0, m_pluginModel->match("name",
                               Settings::instance()->defaultPlaybackFormat(m_pluginModel->service()))), "value")
                               .toMap().value("url").toString()));
           
           if (!isPaused()) {
                m_player->play();
           }
        }
        else {
            setErrorString(tr("No streams found for '%1'").arg(currentTrack() ? currentTrack()->title()
                                                                              : tr("Unknown track")));
            setStatus(Failed);
        }
        
        break;
    case ResourcesRequest::Failed:
        setErrorString(m_pluginModel->errorString());
        setStatus(Failed);
        break;
    default:
        break;
    }
}

void AudioPlayer::onSoundCloudModelStatusChanged(QSoundCloud::StreamsRequest::Status s) {
    switch (s) {
    case QSoundCloud::StreamsRequest::Loading:
        setStatus(Loading);
        break;
    case QSoundCloud::StreamsRequest::Ready:
        setStatus(Loaded);
        
        if (m_soundcloudModel->rowCount() > 0) {
            m_player->setMedia(QUrl(m_soundcloudModel->data(qMax(0, m_soundcloudModel->match("name",
                               Settings::instance()->defaultPlaybackFormat(Resources::SOUNDCLOUD))), "value")
                               .toMap().value("url").toString()));
            
            if (!isPaused()) {
                m_player->play();
            }
        }
        else {
            setErrorString(tr("No streams found for '%1'").arg(currentTrack() ? currentTrack()->title()
                                                                              : tr("Unknown track")));
            setStatus(Failed);
        }
        
        break;
    case QSoundCloud::StreamsRequest::Failed:
        setErrorString(m_soundcloudModel->errorString());
        setStatus(Failed);
        break;
    default:
        break;
    }
}

void AudioPlayer::onStateChanged(QMediaPlayer::State s) {
    switch (s) {
    case QMediaPlayer::StoppedState:
        setStatus(Stopped);
        break;
    case QMediaPlayer::PlayingState:
        setStatus(Playing);
        break;
    case QMediaPlayer::PausedState:
        setStatus(Paused);
        break;
    default:
        break;
    }
}
