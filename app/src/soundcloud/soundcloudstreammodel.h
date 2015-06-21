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

#ifndef SOUNDCLOUDSTREAMMODEL_H
#define SOUNDCLOUDSTREAMMODEL_H

#include "selectionmodel.h"
#include <qsoundcloud/streamsrequest.h>

class SoundCloudStreamModel : public SelectionModel
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QSoundCloud::StreamsRequest::Status status READ status NOTIFY statusChanged)
    
public:
    explicit SoundCloudStreamModel(QObject *parent = 0);
    
    QString errorString() const;
    
    QSoundCloud::StreamsRequest::Status status() const;
    
    Q_INVOKABLE void get(const QString &id);
        
public Q_SLOTS:
    void cancel();
    void reload();

private Q_SLOTS:
    void onRequestFinished();
    
Q_SIGNALS:
    void statusChanged(QSoundCloud::StreamsRequest::Status s);
        
private:
    QSoundCloud::StreamsRequest *m_request;
    
    QString m_id;
};
    
#endif // SOUNDCLOUDSTREAMMODEL_H
