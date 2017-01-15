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

#ifndef MKPLAYLIST_H
#define MKPLAYLIST_H

#include <QObject>
#include <QUrl>

class MKPlaylist : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString artist READ artist NOTIFY artistChanged)
    Q_PROPERTY(QString artistId READ artistId NOTIFY artistIdChanged)
    Q_PROPERTY(QString date READ date NOTIFY dateChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString durationString READ durationString NOTIFY durationChanged)
    Q_PROPERTY(QString genre READ genre NOTIFY genreChanged)
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QUrl largeThumbnailUrl READ largeThumbnailUrl NOTIFY largeThumbnailUrlChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QString service READ service NOTIFY serviceChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(int trackCount READ trackCount NOTIFY trackCountChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    
public:
    explicit MKPlaylist(QObject *parent = 0);
    explicit MKPlaylist(const MKPlaylist *playlist, QObject *parent = 0);
    
    QString artist() const;
    QString artistId() const;
    
    QString date() const;
    
    QString description() const;
    
    qint64 duration() const;
    QString durationString() const;
    
    QString genre() const;
            
    QString id() const;
    
    QUrl largeThumbnailUrl() const;
    QUrl thumbnailUrl() const;
            
    QString service() const;
        
    QString title() const;
            
    int trackCount() const;

    QUrl url() const;
    
    Q_INVOKABLE virtual void loadPlaylist(MKPlaylist *playlist);
    
protected:
    void setArtist(const QString &a);
    void setArtistId(const QString &i);
    
    void setDate(const QString &d);
    
    void setDescription(const QString &d);
    
    void setDuration(qint64 d);
    void setDurationString(const QString &s);
    
    void setGenre(const QString &g);
        
    void setId(const QString &i);
    
    void setLargeThumbnailUrl(const QUrl &u);
    void setThumbnailUrl(const QUrl &u);
    
    void setService(const QString &s);
        
    void setTitle(const QString &t);
        
    void setTrackCount(int c);

    void setUrl(const QUrl &u);
    
Q_SIGNALS:
    void artistChanged();
    void artistIdChanged();
    
    void changed();
    
    void dateChanged();
    
    void descriptionChanged();
    
    void durationChanged();
    
    void genreChanged();
    
    void idChanged();
    
    void largeThumbnailUrlChanged();
    void thumbnailUrlChanged();
    
    void serviceChanged();
    
    void titleChanged();
    
    void trackCountChanged();

    void urlChanged();

private:
    QString m_artist;
    QString m_artistId;
    
    QString m_date;
    
    QString m_description;
    
    qint64 m_duration;
    QString m_durationString;
    
    QString m_genre;
    
    QString m_id;
    
    QUrl m_largeThumbnailUrl;
    QUrl m_thumbnailUrl;
    
    QString m_service;
    
    QString m_title;
    
    int m_trackCount;

    QUrl m_url;
};

#endif // MKPLAYLIST_H
