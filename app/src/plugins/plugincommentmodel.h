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

#ifndef PLUGINCOMMENTMODEL_H
#define PLUGINCOMMENTMODEL_H

#include "plugincomment.h"
#include <QAbstractListModel>

class PluginCommentModel : public QAbstractListModel
{
    Q_OBJECT
    
    Q_PROPERTY(bool canFetchMore READ canFetchMore NOTIFY statusChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(ResourcesRequest::Status status READ status NOTIFY statusChanged)
    
public:
    enum Roles {
        ActionsRole = Qt::UserRole + 1,
        ArtistRole,
        ArtistIdRole,
        BodyRole,
        DateRole,
        ErrorStringRole,
        IdRole,
        ServiceRole,
        StatusRole,
        ThumbnailUrlRole,
        TrackIdRole,
        UrlRole
    };
    
    explicit PluginCommentModel(QObject *parent = 0);
    
    QString errorString() const;
    
    QString service() const;
    void setService(const QString &s);
        
    ResourcesRequest::Status status() const;
    
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    
    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE void fetchMore(const QModelIndex &parent = QModelIndex());
    
    QVariant headerData(int section, Qt::Orientation orientation = Qt::Horizontal, int role = Qt::DisplayRole) const;
    
    QVariant data(const QModelIndex &index, int role) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    Q_INVOKABLE QVariant data(int row, const QByteArray &role) const;
    Q_INVOKABLE QVariantMap itemData(int row) const;
    
    Q_INVOKABLE PluginComment* get(int row) const;
    
    Q_INVOKABLE void list(const QString &resourceId);
    Q_INVOKABLE void search(const QString &query, const QString &order);

public Q_SLOTS:
    void clear();
    void cancel();
    void reload();
    
private Q_SLOTS:
    void onItemChanged();
    void onRequestFinished();
    
Q_SIGNALS:
    void countChanged(int count);
    void serviceChanged();
    void statusChanged(ResourcesRequest::Status s);
    
private:
    void append(PluginComment *comment);
    void insert(int row, PluginComment *comment);
    void remove(int row);
    
    ResourcesRequest* request();
    
    ResourcesRequest *m_request;
    
    QString m_service;
    QString m_resourceId;
    QString m_query;
    QString m_order;
    QString m_next;
        
    QList<PluginComment*> m_items;
    
    QHash<int, QByteArray> m_roles;
};
    
#endif // PLUGINCOMMENTMODEL_H
