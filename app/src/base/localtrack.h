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

#ifndef LOCALTRACK_H
#define LOCALTRACK_H

#include "track.h"
#ifdef FETCH_LOCAL_METADATA
#include <qmobilityglobal.h>

#if QT_VERSION >= 0x050000
QT_USE_DOCGALLERY_NAMESPACE
class QGalleryItemRequest;
#else
QTM_BEGIN_NAMESPACE
class QGalleryItemRequest;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE
#endif
#endif

class LocalTrack : public MKTrack
{
    Q_OBJECT
    
public:
    explicit LocalTrack(QObject *parent = 0);
    explicit LocalTrack(const QUrl &url, QObject *parent = 0);
    explicit LocalTrack(LocalTrack *track, QObject *parent = 0);

    Q_INVOKABLE virtual void loadTrack(const QUrl &url);

#ifdef FETCH_LOCAL_METADATA
private Q_SLOTS:
    void onRequestFinished();
#ifdef MUSIKLOUD_DEBUG
    void onRequestError(int error, const QString &errorString);
#endif
    
private:
    void initRequest();
    
    QGalleryItemRequest *m_request;
#endif
};

#endif // LOCALTRACK_H
