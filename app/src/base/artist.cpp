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

#include "artist.h"

MKArtist::MKArtist(QObject *parent) :
    QObject(parent)
{
}

MKArtist::MKArtist(MKArtist *artist, QObject *parent) :
    QObject(parent),
    m_description(artist->description()),
    m_id(artist->id()),
    m_largeThumbnailUrl(artist->largeThumbnailUrl()),
    m_thumbnailUrl(artist->thumbnailUrl()),
    m_name(artist->name()),
    m_service(artist->service())
{
}

QString MKArtist::description() const {
    return m_description;
}

void MKArtist::setDescription(const QString &d) {
    if (d != description()) {
        m_description = d;
        emit descriptionChanged();
    }
}

QString MKArtist::id() const {
    return m_id;
}

void MKArtist::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit idChanged();
    }
}

QUrl MKArtist::largeThumbnailUrl() const {
    return m_largeThumbnailUrl;
}

void MKArtist::setLargeThumbnailUrl(const QUrl &u) {
    if (u != largeThumbnailUrl()) {
        m_largeThumbnailUrl = u;
        emit largeThumbnailUrlChanged();
    }
}

QUrl MKArtist::thumbnailUrl() const {
    return m_thumbnailUrl;
}

void MKArtist::setThumbnailUrl(const QUrl &u) {
    if (u != thumbnailUrl()) {
        m_thumbnailUrl = u;
        emit thumbnailUrlChanged();
    }
}

QString MKArtist::name() const {
    return m_name;
}

void MKArtist::setName(const QString &u) {
    if (u != name()) {
        m_name = u;
        emit nameChanged();
    }
}

QString MKArtist::service() const {
    return m_service;
}

void MKArtist::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();
    }
}

void MKArtist::loadArtist(MKArtist *artist) {
    setDescription(artist->description());
    setId(artist->id());
    setLargeThumbnailUrl(artist->largeThumbnailUrl());
    setThumbnailUrl(artist->thumbnailUrl());
    setName(artist->name());
    setService(artist->service());
}
