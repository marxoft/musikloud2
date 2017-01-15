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

#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include "track.h"
#include <QAbstractListModel>

class TrackModel : public QAbstractListModel
{
    Q_OBJECT
    
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    
public:
    enum Roles {
        ArtistRole = Qt::UserRole + 1,
        ArtistIdRole,
        DateRole,
        DescriptionRole,
        DownloadableRole,
        DurationRole,
        DurationStringRole,
        FormatRole,
        GenreRole,
        IdRole,
        LargeThumbnailUrlRole,
        PlayCountRole,
        ServiceRole,
        SizeRole,
        SizeStringRole,
        StreamUrlRole,
        ThumbnailUrlRole,
        TitleRole,
        UrlRole
    };
    
    explicit TrackModel(QObject *parent = 0);
    
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    
    QVariant headerData(int section, Qt::Orientation orientation = Qt::Horizontal, int role = Qt::DisplayRole) const;
        
    QVariant data(const QModelIndex &index, int role) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    Q_INVOKABLE QVariant data(int row, const QByteArray &role) const;
    Q_INVOKABLE QVariantMap itemData(int row) const;
    
    Q_INVOKABLE MKTrack* get(int row) const;

private:
    Q_INVOKABLE void append(MKTrack *track);
    Q_INVOKABLE void insert(int row, MKTrack *track);
    Q_INVOKABLE void remove(int row);

private Q_SLOTS:
    void clear();
    
    void onCurrentIndexChanged();
    void onItemChanged();
    
Q_SIGNALS:
    void countChanged(int count);
    
private:
    QList<MKTrack*> m_items;
    
    QHash<int, QByteArray> m_roles;
    
    friend class AudioPlayer;
};
    
#endif // TRACKMODEL_H
