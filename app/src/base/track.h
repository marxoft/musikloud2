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

#ifndef MKTRACK_H
#define MKTRACK_H

#include <QObject>
#include <QUrl>

class MKTrack : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString artist READ artist NOTIFY artistChanged)
    Q_PROPERTY(QString artistId READ artistId NOTIFY artistIdChanged)
    Q_PROPERTY(QString date READ date NOTIFY dateChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(bool downloadable READ isDownloadable NOTIFY downloadableChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString durationString READ durationString NOTIFY durationChanged)
    Q_PROPERTY(QString format READ format NOTIFY formatChanged)
    Q_PROPERTY(QString genre READ genre NOTIFY genreChanged)
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QUrl largeThumbnailUrl READ largeThumbnailUrl NOTIFY largeThumbnailUrlChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(qint64 playCount READ playCount NOTIFY playCountChanged)
    Q_PROPERTY(QString service READ service NOTIFY serviceChanged)
    Q_PROPERTY(qint64 size READ size NOTIFY sizeChanged)
    Q_PROPERTY(QString sizeString READ sizeString NOTIFY sizeChanged)
    Q_PROPERTY(QUrl streamUrl READ streamUrl NOTIFY streamUrlChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    
public:
    explicit MKTrack(QObject *parent = 0);
    explicit MKTrack(MKTrack *track, QObject *parent = 0);
    
    QString artist() const;
    QString artistId() const;
    
    QString date() const;
    
    QString description() const;
    
    bool isDownloadable() const;
    
    qint64 duration() const;
    QString durationString() const;
    
    QString format() const;
    
    QString genre() const;
    
    QString id() const;
    
    QUrl largeThumbnailUrl() const;
    QUrl thumbnailUrl() const;
    
    qint64 playCount() const;
    
    QString service() const;
    
    qint64 size() const;
    QString sizeString() const;
    
    QUrl streamUrl() const;
    
    QString title() const;
    
    QUrl url() const;
    
    Q_INVOKABLE virtual void loadTrack(MKTrack *track);

public Q_SLOTS:
    virtual void played();
    
protected:
    void setArtist(const QString &a);
    void setArtistId(const QString &i);
    
    void setDate(const QString &d);
    
    void setDescription(const QString &d);
    
    void setDownloadable(bool d);
    
    void setDuration(qint64 d);
    void setDurationString(const QString &s);
    
    void setFormat(const QString &f);
    
    void setGenre(const QString &g);
    
    void setId(const QString &id);
    
    void setLargeThumbnailUrl(const QUrl &u);
    void setThumbnailUrl(const QUrl &u);
    
    void setPlayCount(qint64 c);
    
    void setService(const QString &s);
    
    void setSize(qint64 s);
    void setSizeString(const QString &s);
    
    void setStreamUrl(const QUrl &u);
    
    void setTitle(const QString &t);
    
    void setUrl(const QUrl &u);

Q_SIGNALS:
    void artistChanged();
    void artistIdChanged();
    
    void dateChanged();
    
    void descriptionChanged();
    
    void downloadableChanged();
    
    void durationChanged();
    
    void formatChanged();
    
    void genreChanged();
    
    void idChanged();
    
    void largeThumbnailUrlChanged();
    void thumbnailUrlChanged();
    
    void playCountChanged();
    
    void serviceChanged();
    
    void sizeChanged();
    
    void streamUrlChanged();
    
    void titleChanged();
    
    void urlChanged();

protected:
    QString m_artist;
    QString m_artistId;
    
    QString m_date;
    
    QString m_description;
    
    bool m_downloadable;
    
    qint64 m_duration;
    QString m_durationString;
    
    QString m_format;
    
    QString m_genre;
    
    QString m_id;
    
    QUrl m_largeThumbnailUrl;
    QUrl m_thumbnailUrl;
    
    qint64 m_playCount;
    
    QString m_service;
    
    qint64 m_size;
    QString m_sizeString;
    
    QUrl m_streamUrl;
    
    QString m_title;
    
    QUrl m_url;
};
    
#endif // MKTRACK_H
