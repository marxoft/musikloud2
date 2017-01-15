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
#include "logger.h"
#include "plugintrack.h"
#include "resources.h"
#include "settings.h"
#include "soundcloudtrack.h"
#include "utils.h"
#include <QDateTime>
#include <QDir>
#include <QSettings>
#include <algorithm>

AudioPlayerMetaData::AudioPlayerMetaData(QObject *parent) :
    QObject(parent)
{
    connect(AudioPlayer::instance(), SIGNAL(metaDataChanged()), this, SIGNAL(changed()));
}

QString AudioPlayerMetaData::artist() {
    return AudioPlayer::instance()->metaData(Artist).toString();
}

QString AudioPlayerMetaData::artistId() {
    return AudioPlayer::instance()->metaData(ArtistId).toString();
}

QString AudioPlayerMetaData::date() {
    return AudioPlayer::instance()->metaData(Date).toString();
}

QString AudioPlayerMetaData::description() {
    return AudioPlayer::instance()->metaData(Description).toString();
}

qint64 AudioPlayerMetaData::duration() {
    return AudioPlayer::instance()->metaData(Duration).toLongLong();
}

QString AudioPlayerMetaData::durationString() {
    return AudioPlayer::instance()->metaData(DurationString).toString();
}

QString AudioPlayerMetaData::format() {
    return AudioPlayer::instance()->metaData(Format).toString();
}

QString AudioPlayerMetaData::genre() {
    return AudioPlayer::instance()->metaData(Genre).toString();
}

QString AudioPlayerMetaData::id() {
    return AudioPlayer::instance()->metaData(Id).toString();
}

bool AudioPlayerMetaData::isAvailable() {
    return AudioPlayer::instance()->currentTrack() ? true : false;
}

bool AudioPlayerMetaData::isDownloadable() {
    return AudioPlayer::instance()->metaData(Downloadable).toBool();
}

QUrl AudioPlayerMetaData::largeThumbnailUrl() {
    return AudioPlayer::instance()->metaData(LargeThumbnailUrl).toString();
}

QUrl AudioPlayerMetaData::thumbnailUrl() {
    return AudioPlayer::instance()->metaData(ThumbnailUrl).toString();
}

QString AudioPlayerMetaData::service() {
    return AudioPlayer::instance()->metaData(Service).toString();
}

qint64 AudioPlayerMetaData::size() {
    return AudioPlayer::instance()->metaData(Size).toLongLong();
}

QString AudioPlayerMetaData::sizeString() {
    return AudioPlayer::instance()->metaData(SizeString).toString();
}

QUrl AudioPlayerMetaData::streamUrl() {
    return AudioPlayer::instance()->metaData(StreamUrl).toString();
}

QString AudioPlayerMetaData::title() {
    return AudioPlayer::instance()->metaData(Title).toString();
}

QUrl AudioPlayerMetaData::url() {
    return AudioPlayer::instance()->metaData(Url).toString();
}

AudioPlayer* AudioPlayer::self = 0;

AudioPlayer::AudioPlayer(QObject *parent) :
    QObject(parent),
    m_player(new QMediaPlayer(this)),
    m_metaData(0),
    m_queue(new TrackModel(this)),
    m_soundcloudModel(0),
    m_pluginModel(0),
    m_index(-1),
    m_shuffleIndex(0),
    m_repeat(false),
    m_shuffle(false),
    m_sleepTimerDuration(30),
    m_sleepTimerRemaining(0),
    m_stopAfterCurrentTrack(false),
    m_status(Stopped),
    m_metaDataSet(false)
{
    m_sleepTimer.setInterval(60000);
    
    connect(m_player, SIGNAL(bufferStatusChanged(int)), this, SLOT(onBufferStatusChanged(int)));
    connect(m_player, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));
    connect(m_player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onError(QMediaPlayer::Error)));
    connect(m_player, SIGNAL(metaDataChanged()), this, SLOT(onMetaDataChanged()));
    connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_player, SIGNAL(positionChanged(qint64)), this, SIGNAL(positionChanged(qint64)));
    connect(m_player, SIGNAL(seekableChanged(bool)), this, SLOT(onSeekableChanged()));
    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(onStateChanged(QMediaPlayer::State)));
    connect(m_player, SIGNAL(volumeChanged(int)), this, SIGNAL(volumeChanged(int)));
    connect(this, SIGNAL(currentIndexChanged(int)), m_queue, SLOT(onCurrentIndexChanged()));
    connect(m_queue, SIGNAL(countChanged(int)), this, SIGNAL(queueCountChanged(int)));
    connect(&m_sleepTimer, SIGNAL(timeout()), this, SLOT(onSleepTimerTimeout()));
}

AudioPlayer::~AudioPlayer() {
    saveQueue();
    self = 0;
}

AudioPlayer* AudioPlayer::instance() {
    return self ? self : self = new AudioPlayer;
}

AudioPlayerMetaData* AudioPlayer::metaData() {
    if (!m_metaData) {
        m_metaData = new AudioPlayerMetaData(this);
    }
    
    return m_metaData;
}

QVariant AudioPlayer::metaData(AudioPlayerMetaData::MetaData key) const {
    const MKTrack *track = currentTrack();
    
    if (!track) {
        return QVariant();
    }
    
    switch (key) {
    case AudioPlayerMetaData::Artist:
        return track->artist();
    case AudioPlayerMetaData::ArtistId:
        return track->id();
    case AudioPlayerMetaData::Date:
        return track->date();
    case AudioPlayerMetaData::Description:
        return track->description();
    case AudioPlayerMetaData::Downloadable:
        return track->isDownloadable();
    case AudioPlayerMetaData::Duration:
        return track->duration();
    case AudioPlayerMetaData::DurationString:
        return track->durationString();
    case AudioPlayerMetaData::Format:
        return track->format();
    case AudioPlayerMetaData::Genre:
        return track->genre();
    case AudioPlayerMetaData::Id:
        return track->id();
    case AudioPlayerMetaData::LargeThumbnailUrl:
        return track->largeThumbnailUrl();
    case AudioPlayerMetaData::ThumbnailUrl:
        return track->largeThumbnailUrl();
    case AudioPlayerMetaData::Service:
        return track->service();
    case AudioPlayerMetaData::Size:
        return track->size();
    case AudioPlayerMetaData::SizeString:
        return track->sizeString();
    case AudioPlayerMetaData::StreamUrl:
        return track->streamUrl();
    case AudioPlayerMetaData::Title:
        return track->title();
    case AudioPlayerMetaData::Url:
        return track->url();
    default:
        return QVariant();
    }
}

int AudioPlayer::bufferStatus() const {
    return m_player->bufferStatus();
}

int AudioPlayer::currentIndex() const {
    return m_index;
}

void AudioPlayer::setCurrentIndex(int i, bool autoPlay) {
    if ((i >= 0) && (i < queueCount())) {
        m_index = i;
        m_metaDataSet = false;
        autoPlay = (autoPlay) || (isPlaying());
        stop();
        emit currentIndexChanged(i);
        emit metaDataChanged();
        
        if (autoPlay) {
            play();
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
    m_errorString = e;

    if (!e.isEmpty()) {
        Logger::log("AudioPlayer::error(). " + e);
        emit error(e);
    }
}

bool AudioPlayer::isPaused() const {
    return status() == Paused;
}

void AudioPlayer::setPaused(bool p) {
    if ((p) && (isSeekable())) {
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
        if (isSeekable()) {
            pause();
        }
        else {
            stop();
        }
    }
}

void AudioPlayer::togglePlaying() {
    setPlaying(!isPlaying());
}

bool AudioPlayer::isStopped() const {
    switch (status()) {
    case Stopped:
    case Failed:
        return true;
    default:
        return false;
    }
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
    return (m_player->isSeekable()) && (m_player->duration() > 0);
}

bool AudioPlayer::repeatEnabled() const {
    return m_repeat;
}

void AudioPlayer::setRepeatEnabled(bool r) {
    if (r != repeatEnabled()) {
        m_repeat = r;
        emit repeatEnabledChanged(r);
    }
}

bool AudioPlayer::shuffleEnabled() const {
    return m_shuffle;
}

void AudioPlayer::setShuffleEnabled(bool s) {
    if (s != shuffleEnabled()) {
        m_shuffle = s;
        emit shuffleEnabledChanged(s);
        
        if (s) {
            shuffleTracks();
        }
    }
}

int AudioPlayer::sleepTimerDuration() const {
    return m_sleepTimerDuration;
}

void AudioPlayer::setSleepTimerDuration(int d) {
    if ((d != sleepTimerDuration()) && (d > 0)) {
        m_sleepTimerDuration = d;
        emit sleepTimerDurationChanged(d);
    }
}

QString AudioPlayer::sleepTimerDurationString() const {
    return Utils::formatMSecs(sleepTimerDuration() * 60000);
}

bool AudioPlayer::sleepTimerEnabled() const {
    return m_sleepTimer.isActive();
}

void AudioPlayer::setSleepTimerEnabled(bool e) {
    if (e != sleepTimerEnabled()) {
        if (e) {
            setSleepTimerRemaining(sleepTimerDuration());
            m_sleepTimer.start();
        }
        else {
            setSleepTimerRemaining(0);
            m_sleepTimer.stop();
        }
        
        emit sleepTimerEnabledChanged(e);
    }
}

int AudioPlayer::sleepTimerRemaining() const {
    return m_sleepTimerRemaining;
}

void AudioPlayer::setSleepTimerRemaining(int r) {
    if ((r != sleepTimerRemaining()) && (r >= 0)) {
        m_sleepTimerRemaining = r;
        emit sleepTimerRemainingChanged(r);
    }
}

QString AudioPlayer::sleepTimerRemainingString() const {
    return Utils::formatMSecs(sleepTimerRemaining() * 60000);
}

bool AudioPlayer::stopAfterCurrentTrack() const {
    return m_stopAfterCurrentTrack;
}

void AudioPlayer::setStopAfterCurrentTrack(bool s) {
    if (s != stopAfterCurrentTrack()) {
        m_stopAfterCurrentTrack = s;
        emit stopAfterCurrentTrackChanged(s);
    }
}

AudioPlayer::Status AudioPlayer::status() const {
    return m_status;
}

void AudioPlayer::setStatus(AudioPlayer::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

int AudioPlayer::volume() const {
    return m_player->volume();
}

void AudioPlayer::setVolume(int v) {
    m_player->setVolume(v);
}

void AudioPlayer::addTrack(MKTrack *track) {
    m_queue->append(new MKTrack(track, m_queue));
    
    if (currentIndex() < 0) {
        setCurrentIndex(0, false);
    }
    
    if (shuffleEnabled()) {
        shuffleTracks();
    }
}

void AudioPlayer::addTracks(const QList<MKTrack*> &tracks) {
    foreach (MKTrack *track, tracks) {
        m_queue->append(new MKTrack(track, m_queue));
    }
    
    if (currentIndex() < 0) {
        setCurrentIndex(0, false);
    }
    
    if (shuffleEnabled()) {
        shuffleTracks();
    }
}

void AudioPlayer::addTracks(const QVariantList &tracks) {
    foreach (const QVariant &v, tracks) {
        if (MKTrack *track = qobject_cast<MKTrack*>(v.value<QObject*>())) {
            m_queue->append(new MKTrack(track, m_queue));
        }
    }
    
    if (currentIndex() < 0) {
        setCurrentIndex(0, false);
    }
    
    if (shuffleEnabled()) {
        shuffleTracks();
    }
}

void AudioPlayer::removeTrack(int i) {
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
                    return;
                }
            }
            else if (i < queueCount()) {
                next();
                return;
            }
            
            stop();
        }
        
        emit currentIndexChanged(m_index);
        emit metaDataChanged();
    }
}

void AudioPlayer::addLocalFile(const QString &fileName) {
    const QFileInfo info(fileName);
    
    if (info.isDir()) {
        Logger::log("AudioPlayer::addLocalFile(). Listing directory " + fileName, Logger::MediumVerbosity);
        const QDir dir(fileName);
        
        foreach (const QString &name,
                 dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable,
                               QDir::Name | QDir::IgnoreCase | QDir::DirsLast)) {
            addLocalFile(dir.absoluteFilePath(name));
        }
    }
    else if (info.isReadable()) {
        Logger::log("AudioPlayer::addLocalFile(). Checking mimetype for " + fileName, Logger::MediumVerbosity);
        
        if (Utils::isAudioFile(info)) {
            Logger::log("AudioPlayer::addLocalFile(). Adding track " + fileName, Logger::MediumVerbosity);
            m_queue->append(trackFromUrl(QUrl::fromLocalFile(fileName)));
        }
        else {
            Logger::log("AudioPlayer::addLocalFile(). Mimetype not supported for " + fileName, Logger::MediumVerbosity);
        }
    }
    else {
        Logger::log("AudioPlayer::addLocalFile(). File is not readable: " + fileName);
    }
}

MKTrack* AudioPlayer::trackFromUrl(const QUrl &url) {
    MKTrack *track = new MKTrack(m_queue);
    track->setDownloadable(false);
    track->setStreamUrl(url);
    track->setUrl(url);
    
    if (Utils::isLocalFile(url)) {
        const QFileInfo info(Utils::toLocalFile(url));
        const QDir dir = info.dir();
        track->setArtist(dir.dirName());
        track->setArtistId(dir.dirName());
        track->setDate(info.lastModified().toString("dd MMM yyyy"));
        track->setFormat(info.suffix().toUpper());
        track->setGenre(dir.dirName());
        track->setId(info.absoluteFilePath());
        track->setSize(info.size());
        track->setTitle(info.completeBaseName());
        
    }
    else {
        const QString s = url.toString();
        track->setArtist(s.section("/", -2));
        track->setArtistId(track->artist());
        track->setFormat(s.mid(s.lastIndexOf(".") + 1).toUpper());
        track->setGenre(s.section("/", -3));
        track->setId(s);
        track->setTitle(s.section("/", -1).section(".", -2));
    }
    
    return track;
}

int AudioPlayer::addUrl(const QUrl &url) {
    const int count = queueCount();
    
    if (Utils::isLocalFile(url)) {
        addLocalFile(Utils::toLocalFile(url));
    }
    else {
        const QVariantMap resource = Resources::getResourceFromUrl(url.toString());
        
        if (!resource.isEmpty()) {
            if (resource.value("type") == Resources::TRACK) {
                if (resource.value("service") == Resources::SOUNDCLOUD) {
                    m_queue->append(new SoundCloudTrack(resource.value("id").toString(), m_queue));
                }
                else {
                    m_queue->append(new PluginTrack(resource.value("service").toString(),
                                                    resource.value("id").toString(), m_queue));
                }
            }
        }
        else {
            m_queue->append(trackFromUrl(url));
        }
    }
    
    if (currentIndex() < 0) {
        setCurrentIndex(0, false);
    }
    
    if (shuffleEnabled()) {
        shuffleTracks();
    }
    
    return queueCount() - count;
}

int AudioPlayer::addUrls(const QList<QUrl> &urls) {
    const int count = queueCount();
    
    foreach (const QUrl &url, urls) {
        if (Utils::isLocalFile(url)) {
            addLocalFile(Utils::toLocalFile(url));
        }
        else {
            const QVariantMap resource = Resources::getResourceFromUrl(url.toString());
        
            if (!resource.isEmpty()) {
                if (resource.value("type") == Resources::TRACK) {
                    if (resource.value("service") == Resources::SOUNDCLOUD) {
                        m_queue->append(new SoundCloudTrack(resource.value("id").toString(), m_queue));
                    }
                    else {
                        m_queue->append(new PluginTrack(resource.value("service").toString(),
                                                        resource.value("id").toString(), m_queue));
                    }
                }
            }
            else {
                m_queue->append(trackFromUrl(url));
            }
        }
    }
    
    if (currentIndex() < 0) {
        setCurrentIndex(0, false);
    }
    
    if (shuffleEnabled()) {
        shuffleTracks();
    }
    
    return queueCount() - count;
}

void AudioPlayer::clearQueue() {
    stop();
    m_queue->clear();
    m_shuffleOrder.clear();
    m_index = -1;
    m_shuffleIndex = 0;
    emit currentIndexChanged(-1);
    emit metaDataChanged();
}

int AudioPlayer::restoreQueue() {
    QSettings settings(APP_CONFIG_PATH + "queue.conf", QSettings::IniFormat);
    const int size = settings.beginReadArray("queue");
    
    if (size > 0) {
        QList<MKTrack*> tracks;
        
        for (int i = 0; i < size; i++) {
            settings.setArrayIndex(i);
            MKTrack *track = new MKTrack(m_queue);
            track->setArtist(settings.value("artist").toString());
            track->setArtistId(settings.value("artistId").toString());
            track->setDate(settings.value("date").toString());
            track->setDownloadable(settings.value("downloadable", false).toBool());
            track->setDurationString(settings.value("durationString").toString());
            track->setDuration(settings.value("duration").toLongLong());
            track->setFormat(settings.value("format").toString());
            track->setGenre(settings.value("genre").toString());
            track->setId(settings.value("id").toString());
            track->setLargeThumbnailUrl(settings.value("largeThumbnailUrl").toString());
            track->setThumbnailUrl(settings.value("thumbnailUrl").toString());
            track->setPlayCount(settings.value("playCount").toLongLong());
            track->setService(settings.value("service").toString());
            track->setSizeString(settings.value("sizeString").toString());
            track->setSize(settings.value("size").toLongLong());
            track->setStreamUrl(settings.value("streamUrl").toString());
            track->setTitle(settings.value("title").toString());
            track->setUrl(settings.value("url").toString());
            tracks << track;
        }
        
        addTracks(tracks);
    }
    
    settings.endArray();
    setCurrentIndex(qMax(0, settings.value("currentIndex", 0).toInt()), false);
    Logger::log(QString("AudioPlayer::restoreQueue(). %1 tracks restored").arg(size), Logger::LowVerbosity);
    return size;
}

void AudioPlayer::saveQueue() {
    QSettings settings(APP_CONFIG_PATH + "queue.conf", QSettings::IniFormat);
    settings.clear();
    settings.setValue("currentIndex", currentIndex());
    settings.beginWriteArray("queue");
    
    for (int i = 0; i < m_queue->rowCount(); i++) {
        const QModelIndex index = m_queue->index(i);
        settings.setArrayIndex(i);
        settings.setValue("artist", index.data(TrackModel::ArtistRole));
        settings.setValue("artistId", index.data(TrackModel::ArtistIdRole));
        settings.setValue("date", index.data(TrackModel::DateRole));
        settings.setValue("downloadable", index.data(TrackModel::DownloadableRole));
        settings.setValue("duration", index.data(TrackModel::DurationRole));
        settings.setValue("durationString", index.data(TrackModel::DurationStringRole));
        settings.setValue("format", index.data(TrackModel::FormatRole));
        settings.setValue("genre", index.data(TrackModel::GenreRole));
        settings.setValue("id", index.data(TrackModel::IdRole));
        settings.setValue("largeThumbnailUrl", index.data(TrackModel::LargeThumbnailUrlRole).toString());
        settings.setValue("thumbnailUrl", index.data(TrackModel::ThumbnailUrlRole).toString());
        settings.setValue("playCount", index.data(TrackModel::PlayCountRole));
        settings.setValue("service", index.data(TrackModel::ServiceRole));
        settings.setValue("size", index.data(TrackModel::SizeRole));
        settings.setValue("sizeString", index.data(TrackModel::SizeStringRole));
        settings.setValue("streamUrl", index.data(TrackModel::StreamUrlRole).toString());
        settings.setValue("title", index.data(TrackModel::TitleRole));
        settings.setValue("url", index.data(TrackModel::UrlRole).toString());
    }
    
    settings.endArray();
    Logger::log(QString("AudioPlayer::saveQueue(). %1 tracks saved").arg(m_queue->rowCount()),
                Logger::LowVerbosity);
}

void AudioPlayer::next(bool autoPlay) {
    autoPlay = (autoPlay) || (isPlaying());
    
    if (shuffleEnabled()) {
        if (m_shuffleIndex < (m_shuffleOrder.size() - 1)) {
            m_shuffleIndex++;
            setCurrentIndex(m_shuffleOrder.at(m_shuffleIndex), autoPlay);
        }
    }
    else {
        setCurrentIndex(currentIndex() + 1, autoPlay);
    }
}

void AudioPlayer::pause() {
    m_player->pause();
}

void AudioPlayer::play() {
    if (currentIndex() < 0) {
        setCurrentIndex(0, false);
    }
    
    if (isStopped()) {
        if (const MKTrack *track = currentTrack()) {
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
    else {
        m_player->play();
    }
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

void AudioPlayer::playTracks(const QVariantList &tracks) {
    clearQueue();
    addTracks(tracks);
    play();
}

int AudioPlayer::playUrl(const QUrl &url) {
    clearQueue();
    const int added = addUrl(url);
    
    if (added > 0) {
        play();
    }
    
    return added;
}

int AudioPlayer::playUrls(const QList<QUrl> &urls) {
    clearQueue();
    const int added = addUrls(urls);
    
    if (added > 0) {
        play();
    }
    
    return added;
}

void AudioPlayer::previous(bool autoPlay) {
    autoPlay = (autoPlay) || (isPlaying());
    
    if (shuffleEnabled()) {
        if (m_shuffleIndex > 0) {
            m_shuffleIndex--;
            setCurrentIndex(m_shuffleOrder.at(m_shuffleIndex), autoPlay);
        }
    }
    else {
        setCurrentIndex(currentIndex() - 1, autoPlay);
    }
}

void AudioPlayer::stop() {
    m_player->stop();
    m_player->setMedia(QMediaContent());
    emit durationChanged(0);
    emit seekableChanged(false);
    
    if ((m_soundcloudModel) && (m_soundcloudModel->status() == QSoundCloud::StreamsRequest::Loading)) {
        m_soundcloudModel->cancel();
    }
    
    if ((m_pluginModel) && (m_pluginModel->status() == ResourcesRequest::Loading)) {
        m_pluginModel->cancel();
    }
}

void AudioPlayer::shuffleTracks() {
    const int oldSize = m_shuffleOrder.size();
    
    for (int i = oldSize; i < queueCount(); i++) {
        m_shuffleOrder << i;
    }
    
    if (m_shuffleOrder.size() > oldSize) {
        std::random_shuffle(m_shuffleOrder.begin() + oldSize, m_shuffleOrder.end());
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

void AudioPlayer::onBufferStatusChanged(int b) {
    // Workaround as QMediaPlayer does not notify when mediaStatus changes to QMediaPlayer::BufferingMedia
    if (b < 100) {
        setStatus(Buffering);
    }
    
    emit bufferStatusChanged(b);
}

void AudioPlayer::onDurationChanged(qint64 d) {
    if (d > 0) {
        if (MKTrack *track = currentTrack()) {
            track->setDuration(d);
        }
    }
    
    emit durationChanged(d);
    emit seekableChanged(isSeekable());
    emit metaDataChanged();
}

void AudioPlayer::onError(QMediaPlayer::Error e) {
    // Symbian throws an error when setting empty media, so ignore the error in this case
    if ((e != QMediaPlayer::NoError) && (!m_player->media().isNull())) {
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
                next(true);
            }
        }
        
        break;
    default:
        break;
    }
}

void AudioPlayer::onMetaDataChanged() {
    if (m_metaDataSet) {
        return;
    }
    
    MKTrack *track = currentTrack();

    if (!track) {
        emit metaDataChanged();
        return;
    }
    
    const QUrl url = track->url();
    QUrl thumbnailUrl = track->thumbnailUrl();
    
    if (Utils::isLocalFile(url)) {
        if (thumbnailUrl.isEmpty()) {
            thumbnailUrl = Utils::thumbnailUrlForFile(Utils::toLocalFile(url));

            if (!thumbnailUrl.isEmpty()) {
                track->setThumbnailUrl(thumbnailUrl);
                track->setLargeThumbnailUrl(thumbnailUrl);
            }
        }
    }
    
    if (m_player->availableMetaData().isEmpty()) {
        Logger::log("AudioPlayer::onMetaDataChanged(). No metadata available", Logger::HighVerbosity);
        emit metaDataChanged();
        return;
    }
    
    Logger::log("AudioPlayer::onMetaDataChanged(). Metadata is available", Logger::HighVerbosity);
#if QT_VERSION >= 0x050000
    QVariant artist = m_player->metaData("ContributingArtist");
    
    if (artist.isNull()) {
        artist = m_player->metaData("AlbumArtist");
        
        if (artist.isNull()) {
            artist = m_player->metaData("composer");
        }
    }
    
    if (!artist.isNull()) {
        track->setArtist(artist.toString());
    }

    const QVariant date = m_player->metaData("Date");

    if (!date.isNull()) {
        track->setDate(date.toDateTime().toString("dd MMM yyyy"));
    }

    QVariant genre = m_player->metaData("Genre");
    
    if (genre.isNull()) {
        genre = m_player->metaData("AlbumTitle");
    }
    
    if (!genre.isNull()) {
        track->setGenre(genre.toString());
    }

    const QVariant size = m_player->metaData("Size");

    if (!size.isNull()) {
        track->setSize(size.toLongLong());
    }
    
    const QVariant title = m_player->metaData("Title");
    
    if (!title.isNull()) {
        track->setTitle(title.toString());
    }

    if (thumbnailUrl.isEmpty()) {
        QVariant cover = m_player->metaData("coverArtUrlLarge");

        if (cover.isNull()) {
            cover = m_player->metaData("coverArtUrlSmall");
        }

        if (!cover.isNull()) {
            thumbnailUrl = cover.toString();
            track->setThumbnailUrl(thumbnailUrl);
            track->setLargeThumbnailUrl(thumbnailUrl);
        }
    }
#else
    QVariant artist = m_player->metaData(QtMultimediaKit::ContributingArtist);
    
    if (artist.isNull()) {
        artist = m_player->metaData(QtMultimediaKit::AlbumArtist);
        
        if (artist.isNull()) {
            artist = m_player->metaData(QtMultimediaKit::Composer);
        }
    }
    
    if (!artist.isNull()) {
        track->setArtist(artist.toString());
    }

    const QVariant date = m_player->metaData(QtMultimediaKit::Date);

    if (!date.isNull()) {
        track->setDate(date.toDateTime().toString("dd MMM yyyy"));
    }
        
    QVariant genre = m_player->metaData(QtMultimediaKit::Genre);
    
    if (genre.isNull()) {
        genre = m_player->metaData(QtMultimediaKit::AlbumTitle);
    }
    
    if (!genre.isNull()) {
        track->setGenre(genre.toString());
    }

    const QVariant size = m_player->metaData(QtMultimediaKit::Size);

    if (!size.isNull()) {
        track->setSize(size.toLongLong());
    }
    
    const QVariant title = m_player->metaData(QtMultimediaKit::Title);
    
    if (!title.isNull()) {
        track->setTitle(title.toString());
    }

    if (thumbnailUrl.isEmpty()) {
        QVariant cover = m_player->metaData(QtMultimediaKit::CoverArtUrlLarge);

        if (cover.isNull()) {
            cover = m_player->metaData(QtMultimediaKit::CoverArtUrlSmall);
        }

        if (!cover.isNull()) {
            thumbnailUrl = cover.toString();
            track->setThumbnailUrl(thumbnailUrl);
            track->setLargeThumbnailUrl(thumbnailUrl);
        }
    }
#endif
    m_metaDataSet = true;
    emit metaDataChanged();
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
                               Settings::defaultPlaybackFormat(m_pluginModel->service()))), "value")
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

void AudioPlayer::onSeekableChanged() {
    emit seekableChanged(isSeekable());
}

void AudioPlayer::onSleepTimerTimeout() {
    setSleepTimerRemaining(sleepTimerRemaining() - 1);
    
    if (sleepTimerRemaining() == 0) {
        setSleepTimerEnabled(false);
        stop();
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
                               Settings::defaultPlaybackFormat(Resources::SOUNDCLOUD))), "value")
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
