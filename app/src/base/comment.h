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

#ifndef MKCOMMENT_H
#define MKCOMMENT_H

#include <QObject>
#include <QUrl>

class MKComment : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString artist READ artist NOTIFY artistChanged)
    Q_PROPERTY(QString artistId READ artistId NOTIFY artistIdChanged)
    Q_PROPERTY(QString body READ body NOTIFY bodyChanged)
    Q_PROPERTY(QString date READ date NOTIFY dateChanged)
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString service READ service NOTIFY serviceChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QString trackId READ trackId NOTIFY trackIdChanged)
    
public:
    explicit MKComment(QObject *parent = 0);
    explicit MKComment(MKComment *comment, QObject *parent = 0);
    
    QString artist() const;
    QString artistId() const;
    
    QString body() const;
    
    QString date() const;
            
    QString id() const;
                
    QString service() const;
    
    QUrl thumbnailUrl() const;
    
    QString trackId() const;
    
    Q_INVOKABLE virtual void loadComment(MKComment *comment);
    
protected:
    void setArtist(const QString &a);
    void setArtistId(const QString &i);
    
    void setBody(const QString &b);
    
    void setDate(const QString &d);
        
    void setId(const QString &i);
        
    void setService(const QString &s);
    
    void setThumbnailUrl(const QUrl &u);
    
    void setTrackId(const QString &i);
    
Q_SIGNALS:
    void artistChanged();
    void artistIdChanged();
    
    void bodyChanged();
    
    void dateChanged();
    
    void idChanged();
    
    void serviceChanged();
    
    void thumbnailUrlChanged();
    
    void trackIdChanged();

protected:
    QString m_artist;
    QString m_artistId;
    
    QString m_body;
    
    QString m_date;
    
    QString m_id;
    
    QString m_service;
    
    QUrl m_thumbnailUrl;
    
    QString m_trackId;
};

#endif // MKCOMMENT_H
