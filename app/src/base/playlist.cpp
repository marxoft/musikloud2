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

#include "playlist.h"
#include "utils.h"

MKPlaylist::MKPlaylist(QObject *parent) :
    QObject(parent),
    m_duration(0),
    m_trackCount(0)
{
}

MKPlaylist::MKPlaylist(const MKPlaylist *playlist, QObject *parent) :
    QObject(parent),
    m_artist(playlist->artist()),
    m_artistId(playlist->artistId()),
    m_date(playlist->date()),
    m_description(playlist->description()),
    m_duration(playlist->duration()),
    m_durationString(playlist->durationString()),
    m_genre(playlist->genre()),
    m_id(playlist->id()),
    m_largeThumbnailUrl(playlist->largeThumbnailUrl()),
    m_thumbnailUrl(playlist->thumbnailUrl()),
    m_service(playlist->service()),
    m_title(playlist->title()),
    m_trackCount(playlist->trackCount()),
    m_url(playlist->url())
{
}

QString MKPlaylist::artist() const {
    return m_artist;
}

void MKPlaylist::setArtist(const QString &a) {
    if (a != artist()) {
        m_artist = a;
        emit changed();
        emit artistChanged();
    }
}

QString MKPlaylist::artistId() const {
    return m_artistId;
}

void MKPlaylist::setArtistId(const QString &i) {
    if (i != artistId()) {
        m_artistId = i;
        emit changed();
        emit artistIdChanged();
    }
}

QString MKPlaylist::date() const {
    return m_date;
}

void MKPlaylist::setDate(const QString &d) {
    if (d != date()) {
        m_date = d;
        emit changed();
        emit dateChanged();
    }
}

QString MKPlaylist::description() const {
    return m_description;
}

void MKPlaylist::setDescription(const QString &d) {
    if (d != description()) {
        m_description = d;
        emit changed();
        emit descriptionChanged();
    }
}

qint64 MKPlaylist::duration() const {
    return m_duration;
}

void MKPlaylist::setDuration(qint64 d) {
    if (d != duration()) {
        m_duration = d;
        m_durationString = Utils::formatMSecs(d);
        emit changed();
        emit durationChanged();
    }
}

QString MKPlaylist::durationString() const {
    return m_durationString;
}

void MKPlaylist::setDurationString(const QString &s) {
    if (s != durationString()) {
        m_durationString = s;
        emit changed();
        emit durationChanged();
    }
}

QString MKPlaylist::genre() const {
    return m_genre;
}

void MKPlaylist::setGenre(const QString &g) {
    if (g != genre()) {
        m_genre = g;
        emit changed();
        emit genreChanged();
    }
}

QString MKPlaylist::id() const {
    return m_id;
}

void MKPlaylist::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit changed();
        emit idChanged();
    }
}

QUrl MKPlaylist::largeThumbnailUrl() const {
    return m_largeThumbnailUrl;
}

void MKPlaylist::setLargeThumbnailUrl(const QUrl &u) {
    if (u != largeThumbnailUrl()) {
        m_largeThumbnailUrl = u;
        emit changed();
        emit largeThumbnailUrlChanged();
    }
}

QUrl MKPlaylist::thumbnailUrl() const {
    return m_thumbnailUrl;
}

void MKPlaylist::setThumbnailUrl(const QUrl &u) {
    if (u != thumbnailUrl()) {
        m_thumbnailUrl = u;
        emit changed();
        emit thumbnailUrlChanged();
    }
}

QString MKPlaylist::service() const {
    return m_service;
}

void MKPlaylist::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit changed();
        emit serviceChanged();
    }
}

QString MKPlaylist::title() const {
    return m_title;
}

void MKPlaylist::setTitle(const QString &t) {
    if (t != title()) {
        m_title = t;
        emit changed();
        emit titleChanged();
    }
}

int MKPlaylist::trackCount() const {
    return m_trackCount;
}

void MKPlaylist::setTrackCount(int c) {
    if (c != trackCount()) {
        m_trackCount = c;
        emit changed();
        emit trackCountChanged();
    }
}

QUrl MKPlaylist::url() const {
    return m_url;
}

void MKPlaylist::setUrl(const QUrl &u) {
    if (u != url()) {
        m_url = u;
        emit changed();
        emit urlChanged();
    }
}

void MKPlaylist::loadPlaylist(MKPlaylist *playlist) {
    setArtist(playlist->artist());
    setArtistId(playlist->artistId());
    setDate(playlist->date());
    setDescription(playlist->description());
    setDuration(playlist->duration());
    setGenre(playlist->genre());
    setId(playlist->id());
    setLargeThumbnailUrl(playlist->largeThumbnailUrl());
    setThumbnailUrl(playlist->thumbnailUrl());
    setService(playlist->service());
    setTitle(playlist->title());
    setTrackCount(playlist->trackCount());
    setUrl(playlist->url());
    
    if (duration() == 0) {
        setDurationString(playlist->durationString());
    }
}
