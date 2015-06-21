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

#include "track.h"
#include "utils.h"

MKTrack::MKTrack(QObject *parent) :
    QObject(parent),
    m_artist(tr("Unknown artist")),
    m_date(tr("Unknown date")),
    m_description(tr("No description")),
    m_downloadable(true),
    m_duration(0),
    m_durationString("--:--"),
    m_format(tr("Unknown format")),
    m_genre(tr("Unknown genre")),
    m_playCount(0),
    m_size(0),
    m_title(tr("Unknown title"))
{
}

MKTrack::MKTrack(const QUrl &url, QObject *parent) :
    QObject(parent),
    m_artist(tr("Unknown artist")),
    m_date(tr("Unknown date")),
    m_description(tr("No description")),
    m_downloadable(url.scheme() != "file"),
    m_duration(0),
    m_durationString("--:--"),
    m_format(url.path().section('.', -1).toUpper()),
    m_genre(tr("Unknown genre")),
    m_id(url.path()),
    m_playCount(0),
    m_size(0),
    m_streamUrl(url),
    m_title(url.path().section('/', -1).section('.', 0, -2)),
    m_url(url)
{
}

MKTrack::MKTrack(MKTrack *track, QObject *parent) :
    QObject(parent),
    m_artist(track->artist()),
    m_artistId(track->artistId()),
    m_date(track->date()),
    m_description(track->description()),
    m_downloadable(track->isDownloadable()),
    m_duration(track->duration()),
    m_durationString(track->durationString()),
    m_format(track->format()),
    m_genre(track->genre()),
    m_id(track->id()),
    m_largeThumbnailUrl(track->largeThumbnailUrl()),
    m_thumbnailUrl(track->thumbnailUrl()),
    m_playCount(track->playCount()),
    m_service(track->service()),
    m_size(track->size()),
    m_sizeString(track->sizeString()),
    m_streamUrl(track->streamUrl()),
    m_title(track->title()),
    m_url(track->url())
{
}

QString MKTrack::artist() const {
    return m_artist;
}

void MKTrack::setArtist(const QString &a) {
    if (a != artist()) {
        m_artist = a;
        emit artistChanged();
    }
}

QString MKTrack::artistId() const {
    return m_artistId;
}

void MKTrack::setArtistId(const QString &i) {
    if (i != artistId()) {
        m_artistId = i;
        emit artistIdChanged();
    }
}

QString MKTrack::date() const {
    return m_date;
}

void MKTrack::setDate(const QString &d) {
    if (d != date()) {
        m_date = d;
        emit dateChanged();
    }
}

QString MKTrack::description() const {
    return m_description;
}

void MKTrack::setDescription(const QString &d) {
    if (d != description()) {
        m_description = d;
        emit descriptionChanged();
    }
}

bool MKTrack::isDownloadable() const {
    return m_downloadable;
}

void MKTrack::setDownloadable(bool d) {
    if (d != isDownloadable()) {
        m_downloadable = d;
        emit downloadableChanged();
    }
}

qint64 MKTrack::duration() const {
    return m_duration;
}

void MKTrack::setDuration(qint64 d) {
    if (d != duration()) {
        m_duration = d;
        m_durationString = Utils::formatMSecs(d);
        emit durationChanged();
    }
}

QString MKTrack::durationString() const {
    return m_durationString;
}

void MKTrack::setDurationString(const QString &s) {
    if (s != durationString()) {
        m_durationString = s;
        emit durationChanged();
    }
}

QString MKTrack::format() const {
    return m_format;
}

void MKTrack::setFormat(const QString &f) {
    if (f != format()) {
        m_format = f;
        emit formatChanged();
    }
}

QString MKTrack::genre() const {
    return m_genre;
}

void MKTrack::setGenre(const QString &g) {
    if (g != genre()) {
        m_genre = g;
        emit genreChanged();
    }
}

QString MKTrack::id() const {
    return m_id;
}

void MKTrack::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit idChanged();
    }
}

QUrl MKTrack::largeThumbnailUrl() const {
    return m_largeThumbnailUrl;
}

void MKTrack::setLargeThumbnailUrl(const QUrl &u) {
    if (u != largeThumbnailUrl()) {
        m_largeThumbnailUrl = u;
        emit largeThumbnailUrlChanged();
    }
}

QUrl MKTrack::thumbnailUrl() const {
    return m_thumbnailUrl;
}

void MKTrack::setThumbnailUrl(const QUrl &u) {
    if (u != thumbnailUrl()) {
        m_thumbnailUrl = u;
        emit thumbnailUrlChanged();
    }
}

qint64 MKTrack::playCount() const {
    return m_playCount;
}

void MKTrack::setPlayCount(qint64 c) {
    if (c != playCount()) {
        m_playCount = c;
        emit playCountChanged();
    }
}

QString MKTrack::service() const {
    return m_service;
}

void MKTrack::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();
    }
}

qint64 MKTrack::size() const {
    return m_size;
}

void MKTrack::setSize(qint64 s) {
    if (s != size()) {
        m_size = s;
        m_sizeString = Utils::formatBytes(s);
        emit sizeChanged();
    }
}

QString MKTrack::sizeString() const {
    return m_sizeString;
}

void MKTrack::setSizeString(const QString &s) {
    if (s != sizeString()) {
        m_sizeString = s;
        emit sizeChanged();
    }
}

QUrl MKTrack::streamUrl() const {
    return m_streamUrl;
}

void MKTrack::setStreamUrl(const QUrl &u) {
    if (u != streamUrl()) {
        m_streamUrl = u;
        emit streamUrlChanged();
    }
}

QString MKTrack::title() const {
    return m_title;
}

void MKTrack::setTitle(const QString &t) {
    if (t != title()) {
        m_title = t;
        emit titleChanged();
    }
}

QUrl MKTrack::url() const {
    return m_url;
}

void MKTrack::setUrl(const QUrl &u) {
    if (u != url()) {
        m_url = u;
        emit urlChanged();
    }
}

void MKTrack::loadTrack(MKTrack *track) {
    setArtist(track->artist());
    setArtistId(track->artistId());
    setDate(track->date());
    setDescription(track->description());
    setDuration(track->duration());
    setFormat(track->format());
    setGenre(track->genre());
    setId(track->id());
    setLargeThumbnailUrl(track->largeThumbnailUrl());
    setThumbnailUrl(track->thumbnailUrl());
    setPlayCount(track->playCount());
    setService(track->service());
    setSize(track->size());
    setStreamUrl(track->streamUrl());
    setTitle(track->title());
    setUrl(track->url());
    
    if (duration() == 0) {
        setDurationString(track->durationString());
    }
    
    if (size() == 0) {
        setSizeString(track->sizeString());
    }
}

void MKTrack::played() {
    setPlayCount(playCount() + 1);
}
