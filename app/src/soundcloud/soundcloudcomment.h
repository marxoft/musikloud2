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

#ifndef SOUNDCLOUDCOMMENT_H
#define SOUNDCLOUDCOMMENT_H

#include "comment.h"
#include <qsoundcloud/resourcesrequest.h>

class SoundCloudComment : public MKComment
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged);
    Q_PROPERTY(QSoundCloud::ResourcesRequest::Status status READ status NOTIFY statusChanged)

public:
    explicit SoundCloudComment(QObject *parent = 0);
    explicit SoundCloudComment(const QString &id, QObject *parent = 0);
    explicit SoundCloudComment(const QVariantMap &comment, QObject *parent = 0);
    explicit SoundCloudComment(const SoundCloudComment *comment, QObject *parent = 0);
    
    QString errorString() const;
    
    QSoundCloud::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadComment(const QString &id);
    Q_INVOKABLE void loadComment(const QVariantMap &comment);

public Q_SLOTS:
    void addComment();
    void addComment(const QVariantMap &comment);
    void cancel();
    
private:
    void initRequest();

private Q_SLOTS:
    void onCommentRequestFinished();
    void onAddCommentRequestFinished();

Q_SIGNALS:
    void statusChanged(QSoundCloud::ResourcesRequest::Status s);
    
private:
    QSoundCloud::ResourcesRequest *m_request;
};
#endif // SOUNDCLOUDCOMMENT_H
