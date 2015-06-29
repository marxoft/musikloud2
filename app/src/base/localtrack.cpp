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

#include "localtrack.h"
#ifdef MUSIKLOUD_DEBUG
#include <QDebug>
#endif
#ifdef FETCH_LOCAL_METADATA
#include <QGalleryItemRequest>
#include <QDocumentGallery>
#include <QDateTime>

static const QStringList PROPERTY_NAMES = QStringList() << QDocumentGallery::artist << QDocumentGallery::duration
                                                        << QDocumentGallery::fileSize << QDocumentGallery::genre
                                                        << QDocumentGallery::lastModified << QDocumentGallery::playCount
                                                        << QDocumentGallery::title;
#endif

LocalTrack::LocalTrack(QObject *parent) :
    MKTrack(parent)
#ifdef FETCH_LOCAL_METADATA
    ,m_request(0)
#endif
{
    setDownloadable(false);
}

LocalTrack::LocalTrack(const QUrl &url, QObject *parent) :
    MKTrack(parent)
#ifdef FETCH_LOCAL_METADATA
    ,m_request(0)
#endif
{
    setDownloadable(false);
    loadTrack(url);
}

LocalTrack::LocalTrack(LocalTrack *track, QObject *parent) :
    MKTrack(track, parent)
#ifdef FETCH_LOCAL_METADATA
    ,m_request(0)
#endif
{
}

void LocalTrack::loadTrack(const QUrl &url) {
    setFormat(url.path().section('.', -1).toUpper());
    setStreamUrl(url);
    setTitle(url.path().section('/', -1).section('.', 0, -2));
    setUrl(url);
#ifdef FETCH_LOCAL_METADATA
    initRequest();
    m_request->setItemId("localtagfs::music/songs/" + url.path().replace("/", "%2F"));
    m_request->execute();
#endif
}

#ifdef FETCH_LOCAL_METADATA
void LocalTrack::initRequest() {
    if (!m_request) {
        m_request = new QGalleryItemRequest(new QDocumentGallery(this), this);
        m_request->setPropertyNames(PROPERTY_NAMES);
        connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
#ifdef MUSIKLOUD_DEBUG
        connect(m_request, SIGNAL(error(int, QString)), this, SLOT(onRequestError(int, QString)));
#endif
    }
}

void LocalTrack::onRequestFinished() {
    if (m_request->isValid()) {
        setArtist(m_request->metaData(QDocumentGallery::artist).toString());
        setDate(m_request->metaData(QDocumentGallery::lastModified).toDateTime().toString("dd MMM yyyy"));
        setDuration(m_request->metaData(QDocumentGallery::duration).toLongLong());
        setFormat(m_request->metaData(QDocumentGallery::fileExtension).toString().toUpper());
        setGenre(m_request->metaData(QDocumentGallery::genre).toString());
        setId(m_request->itemId().toString());
        setPlayCount(m_request->metaData(QDocumentGallery::playCount).toLongLong());
        setSize(m_request->metaData(QDocumentGallery::fileSize).toLongLong());
        setTitle(m_request->metaData(QDocumentGallery::title).toString());
    }
#ifdef MUSIKLOUD_DEBUG
    else {
        qDebug() << "LocalTrack::onRequestFinished: Item is invalid" << m_request->itemId();
    }
#endif
}

#ifdef MUSIKLOUD_DEBUG
void LocalTrack::onRequestError(int error, const QString &errorString) {
    qDebug() << "LocalTrack::onRequestError" << m_request->itemId() << error << errorString;
}
#endif
#endif
