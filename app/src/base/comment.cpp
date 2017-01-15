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

#include "comment.h"

MKComment::MKComment(QObject *parent) :
    QObject(parent)
{
}

MKComment::MKComment(const MKComment *comment, QObject *parent) :
    QObject(parent),
    m_artist(comment->artist()),
    m_artistId(comment->artistId()),
    m_body(comment->body()),
    m_date(comment->date()),
    m_id(comment->id()),
    m_service(comment->service()),
    m_thumbnailUrl(comment->thumbnailUrl()),
    m_trackId(comment->trackId()),
    m_url(comment->url())
{
}

QString MKComment::artist() const {
    return m_artist;
}

void MKComment::setArtist(const QString &a) {
    if (a != artist()) {
        m_artist = a;
        emit changed();
        emit artistChanged();
    }
}

QString MKComment::artistId() const {
    return m_artistId;
}

void MKComment::setArtistId(const QString &i) {
    if (i != artistId()) {
        m_artistId = i;
        emit changed();
        emit artistIdChanged();
    }
}

QString MKComment::body() const {
    return m_body;
}

void MKComment::setBody(const QString &d) {
    if (d != body()) {
        m_body = d;
        emit changed();
        emit bodyChanged();
    }
}

QString MKComment::date() const {
    return m_date;
}

void MKComment::setDate(const QString &d) {
    if (d != date()) {
        m_date = d;
        emit changed();
        emit dateChanged();
    }
}

QString MKComment::id() const {
    return m_id;
}

void MKComment::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit changed();
        emit idChanged();
    }
}

QString MKComment::service() const {
    return m_service;
}

void MKComment::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit changed();
        emit serviceChanged();
    }
}

QUrl MKComment::thumbnailUrl() const {
    return m_thumbnailUrl;
}

void MKComment::setThumbnailUrl(const QUrl &u) {
    if (u != thumbnailUrl()) {
        m_thumbnailUrl = u;
        emit changed();
        emit thumbnailUrlChanged();
    }
}

QString MKComment::trackId() const {
    return m_trackId;
}

void MKComment::setTrackId(const QString &i) {
    if (i != trackId()) {
        m_trackId = i;
        emit changed();
        emit trackIdChanged();
    }
}

QUrl MKComment::url() const {
    return m_url;
}

void MKComment::setUrl(const QUrl &u) {
    if (u != url()) {
        m_url = u;
        emit changed();
        emit urlChanged();
    }
}

void MKComment::loadComment(MKComment *comment) {
    setArtist(comment->artist());
    setArtistId(comment->artistId());
    setBody(comment->body());
    setDate(comment->date());
    setId(comment->id());
    setService(comment->service());
    setThumbnailUrl(comment->thumbnailUrl());
    setTrackId(comment->trackId());
    setUrl(comment->url());
}
