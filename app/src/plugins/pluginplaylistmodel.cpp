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

#include "pluginplaylistmodel.h"
#include "logger.h"
#include "pluginmanager.h"
#include "resources.h"

PluginPlaylistModel::PluginPlaylistModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(0)
{
    m_roles[ActionsRole] = "actions";
    m_roles[ArtistRole] = "artist";
    m_roles[ArtistIdRole] = "artistId";
    m_roles[DateRole] = "date";
    m_roles[DescriptionRole] = "description";
    m_roles[DurationRole] = "duration";
    m_roles[DurationStringRole] = "durationString";
    m_roles[ErrorStringRole] = "errorString";
    m_roles[GenreRole] = "genre";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[ServiceRole] = "service";
    m_roles[StatusRole] = "status";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
    m_roles[TrackCountRole] = "trackCount";
    m_roles[TracksIdRole] = "tracksId";
    m_roles[UrlRole] = "url";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
}

QString PluginPlaylistModel::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

QString PluginPlaylistModel::service() const {
    return m_service;
}

void PluginPlaylistModel::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();

        clear();

        if (m_request) {
            m_request->deleteLater();
            m_request = 0;
        }
    }
}

ResourcesRequest::Status PluginPlaylistModel::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> PluginPlaylistModel::roleNames() const {
    return m_roles;
}
#endif

int PluginPlaylistModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_items.size();
}

int PluginPlaylistModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 4;
}

bool PluginPlaylistModel::canFetchMore(const QModelIndex &parent) const {
    return (!parent.isValid()) && (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginPlaylistModel::fetchMore(const QModelIndex &parent) {
    if (!canFetchMore(parent)) {
        return;
    }

    if (ResourcesRequest *r = request()) {
        r->list(Resources::PLAYLIST, m_next);
        emit statusChanged(status());
    }
}

QVariant PluginPlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((orientation != Qt::Horizontal) || (role != Qt::DisplayRole)) {
        return QVariant();
    }
    
    switch (section) {
    case 0:
        return tr("Title");
    case 1:
        return tr("Artist");
    case 2:
        return tr("Tracks");
    case 3:
        return tr("Duration");
    default:
        return QVariant();
    }
}

QVariant PluginPlaylistModel::data(const QModelIndex &index, int role) const {
    if (const PluginPlaylist *playlist = get(index.row())) {
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case 0:
                return playlist->title();
            case 1:
                return playlist->artist();
            case 2:
                return playlist->trackCount();
            case 3:
                return playlist->durationString();
            default:
                return QVariant();
            }
        }
        
        return playlist->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> PluginPlaylistModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const PluginPlaylist *playlist = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = playlist->property(iterator.value());
        }
    }
    
    return map;
}

QVariant PluginPlaylistModel::data(int row, const QByteArray &role) const {
    if (const PluginPlaylist *playlist = get(row)) {
        return playlist->property(role);
    }
    
    return QVariant();
}

QVariantMap PluginPlaylistModel::itemData(int row) const {
    QVariantMap map;
    
    if (const PluginPlaylist *playlist = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
            map[role] = playlist->property(role);
        }
    }
    
    return map;
}

PluginPlaylist* PluginPlaylistModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void PluginPlaylistModel::list(const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginPlaylistModel::list(). Resource ID: " + resourceId, Logger::MediumVerbosity);
    clear();
    m_resourceId = resourceId;
    m_query = QString();

    if (ResourcesRequest *r = request()) {
        r->list(Resources::PLAYLIST, resourceId);
        emit statusChanged(status());
    }
}

void PluginPlaylistModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log(QString("PluginPlaylistModel::search(). Query: %1, Order: %2").arg(query).arg(order),
                Logger::MediumVerbosity);
    clear();
    m_resourceId = QString();
    m_query = query;
    m_order = order;

    if (ResourcesRequest *r = request()) {
        r->search(Resources::PLAYLIST, query, order);
        emit statusChanged(status());
    }
}

void PluginPlaylistModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_next = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void PluginPlaylistModel::cancel() {
    m_request->cancel();
}

void PluginPlaylistModel::reload() {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginPlaylistModel::reload(). Resource ID: " + m_resourceId, Logger::MediumVerbosity);
    clear();

    if (ResourcesRequest *r = request()) {
        if (m_query.isEmpty()) {
            r->list(Resources::PLAYLIST, m_resourceId);
        }
        else {
            r->search(Resources::PLAYLIST, m_query, m_order);
        }
        
        emit statusChanged(status());
    }
}

void PluginPlaylistModel::append(PluginPlaylist *playlist) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    connect(playlist, SIGNAL(changed()), this, SLOT(onItemChanged()));
    m_items << playlist;
    endInsertRows();
}

void PluginPlaylistModel::insert(int row, PluginPlaylist *playlist) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        connect(playlist, SIGNAL(changed()), this, SLOT(onItemChanged()));
        m_items.insert(row, playlist);
        endInsertRows();
    }
    else {
        append(playlist);
    }
}

void PluginPlaylistModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

ResourcesRequest* PluginPlaylistModel::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginPlaylistModel::onItemChanged() {
    const int row = m_items.indexOf(qobject_cast<PluginPlaylist*>(sender()));
    
    if (row != -1) {
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

void PluginPlaylistModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_next = result.value("next").toString();
            const QVariantList list = result.value("items").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (const QVariant &item, list) {
                PluginPlaylist *playlist = new PluginPlaylist(service(), item.toMap(), this);
                connect(playlist, SIGNAL(changed()), this, SLOT(onItemChanged()));
                m_items << playlist;
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    else {
        Logger::log("PluginPlaylistModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}
