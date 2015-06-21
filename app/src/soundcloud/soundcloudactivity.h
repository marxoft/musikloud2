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

#ifndef SOUNDCLOUDACTIVITY_H
#define SOUNDCLOUDACTIVITY_H

#include <qsoundcloud/resourcesrequest.h>
#include <QUrl>

class SoundCloudActivity : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString activityType READ activityType NOTIFY activityTypeChanged)
    Q_PROPERTY(QString activityTypeString READ activityTypeString NOTIFY activityTypeChanged)
    Q_PROPERTY(QString artist READ artist NOTIFY artistChanged)
    Q_PROPERTY(QString artistId READ artistId NOTIFY artistIdChanged)
    Q_PROPERTY(QString date READ date NOTIFY dateChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString originId READ originId NOTIFY originIdChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QSoundCloud::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

public:
    explicit SoundCloudActivity(QObject *parent = 0);
    explicit SoundCloudActivity(const QString &id, QObject *parent = 0);
    explicit SoundCloudActivity(const QVariantMap &activity, QObject *parent = 0);
    explicit SoundCloudActivity(SoundCloudActivity *activity, QObject *parent = 0);
    
    QString activityType() const;
    QString activityTypeString() const;
    
    QString artist() const;
    QString artistId() const;
        
    QString date() const;
    
    QString description() const;
    
    QString errorString() const;
    
    QString id() const;
    
    QString originId() const;
    
    QUrl thumbnailUrl() const;
    
    QSoundCloud::ResourcesRequest::Status status() const;
    
    QString title() const;
    
    Q_INVOKABLE void loadActivity(const QString &id);
    Q_INVOKABLE void loadActivity(const QVariantMap &activity);
    Q_INVOKABLE void loadActivity(SoundCloudActivity *activity);

private:
    void initRequest();
    
    void setActivityType(const QString &t);
    
    void setArtist(const QString &a);
    void setArtistId(const QString &i);
        
    void setDate(const QString &d);
    
    void setDescription(const QString &d);
    
    void setId(const QString &i);
    
    void setOriginId(const QString &i);
    
    void setThumbnailUrl(const QUrl &u);
    
    void setTitle(const QString &t);

private Q_SLOTS:
    void onActivityRequestFinished();

Q_SIGNALS:
    void activityTypeChanged();
    
    void artistChanged();
    void artistIdChanged();
        
    void dateChanged();
    
    void descriptionChanged();
    
    void idChanged();
    
    void originIdChanged();
    
    void thumbnailUrlChanged();
    
    void statusChanged(QSoundCloud::ResourcesRequest::Status s);
    
    void titleChanged();

private:
    QSoundCloud::ResourcesRequest *m_request;
    
    QString m_type;
    QString m_typeString;
    
    QString m_artist;
    QString m_artistId;
        
    QString m_date;
    
    QString m_description;
    
    QString m_id;
    
    QString m_originId;
    
    QUrl m_thumbnailUrl;
    
    QString m_title;
};

#endif // SOUNDCLOUDACTIVITY_H
