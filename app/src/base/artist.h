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

#ifndef MKARTIST_H
#define MKARTIST_H

#include <QObject>
#include <QUrl>

class MKArtist : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QUrl largeThumbnailUrl READ largeThumbnailUrl NOTIFY largeThumbnailUrlChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString service READ service NOTIFY serviceChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    
public:
    explicit MKArtist(QObject *parent = 0);
    explicit MKArtist(const MKArtist *artist, QObject *parent = 0);
    
    QString description() const;
    
    QString id() const;
    
    QUrl largeThumbnailUrl() const;
    QUrl thumbnailUrl() const;
    
    QString name() const;
    
    QString service() const;

    QUrl url() const;
    
    Q_INVOKABLE virtual void loadArtist(MKArtist *artist);

protected:
    void setDescription(const QString &d);
    
    void setId(const QString &i);
    
    void setLargeThumbnailUrl(const QUrl &u);
    void setThumbnailUrl(const QUrl &u);
    
    void setName(const QString &u);
    
    void setService(const QString &s);

    void setUrl(const QUrl &u);

Q_SIGNALS:
    void changed();
    
    void descriptionChanged();
    
    void idChanged();
    
    void largeThumbnailUrlChanged();
    void thumbnailUrlChanged();
    
    void nameChanged();
    
    void serviceChanged();

    void urlChanged();

private:    
    QString m_description;
    
    QString m_id;
    
    QUrl m_largeThumbnailUrl;
    QUrl m_thumbnailUrl;
    
    QString m_name;
    
    QString m_service;

    QUrl m_url;
};

#endif // MKARTIST_H
