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

#include "pluginartistmodel.h"
#include "logger.h"
#include "pluginmanager.h"
#include "resources.h"

PluginArtistModel::PluginArtistModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(0)
{
    m_roles[ActionsRole] = "actions";
    m_roles[DescriptionRole] = "description";
    m_roles[ErrorStringRole] = "errorString";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[NameRole] = "name";
    m_roles[PlaylistsIdRole] = "playlistsId";
    m_roles[ServiceRole] = "service";
    m_roles[StatusRole] = "status";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TracksIdRole] = "tracksId";
    m_roles[UrlRole] = "url";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
}

QString PluginArtistModel::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

QString PluginArtistModel::service() const {
    return m_service;
}

void PluginArtistModel::setService(const QString &s) {
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

ResourcesRequest::Status PluginArtistModel::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> PluginArtistModel::roleNames() const {
    return m_roles;
}
#endif

int PluginArtistModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_items.size();
}

int PluginArtistModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 2;
}

bool PluginArtistModel::canFetchMore(const QModelIndex &parent) const {
    return (!parent.isValid()) && (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginArtistModel::fetchMore(const QModelIndex &parent) {
    if (!canFetchMore(parent)) {
        return;
    }

    if (ResourcesRequest *r = request()) {
        r->list(Resources::ARTIST, m_next);
        emit statusChanged(status());
    }
}

QVariant PluginArtistModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((orientation != Qt::Horizontal) || (role != Qt::DisplayRole)) {
        return QVariant();
    }
    
    switch (section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Description");
    default:
        return QVariant();
    }
}

QVariant PluginArtistModel::data(const QModelIndex &index, int role) const {
    if (const PluginArtist *artist = get(index.row())) {
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case 0:
                return artist->name();
            case 1:
                return artist->description();
            default:
                return QVariant();
            }
        }
        
        return artist->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> PluginArtistModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const PluginArtist *artist = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = artist->property(iterator.value());
        }
    }
    
    return map;
}

QVariant PluginArtistModel::data(int row, const QByteArray &role) const {
    if (const PluginArtist *artist = get(row)) {        
        return artist->property(role);
    }
    
    return QVariant();
}

QVariantMap PluginArtistModel::itemData(int row) const {
    QVariantMap map;
    
    if (const PluginArtist *artist = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
            map[role] = artist->property(role);
        }
    }
    
    return map;
}

PluginArtist* PluginArtistModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void PluginArtistModel::list(const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginArtistModel::list(). Resource ID: " + resourceId, Logger::MediumVerbosity);
    clear();
    m_resourceId = resourceId;
    m_query = QString();

    if (ResourcesRequest *r = request()) {
        r->list(Resources::ARTIST, resourceId);
        emit statusChanged(status());
    }
}

void PluginArtistModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log(QString("PluginArtistModel::search(). Query: %1, Order: %2").arg(query).arg(order),
                Logger::MediumVerbosity);
    clear();
    m_resourceId = QString();
    m_query = query;
    m_order = order;

    if (ResourcesRequest *r = request()) {
        r->search(Resources::ARTIST, query, order);
        emit statusChanged(status());
    }
}

void PluginArtistModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_next = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void PluginArtistModel::cancel() {
    m_request->cancel();
}

void PluginArtistModel::reload() {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginArtistModel::reload(). Resource ID: " + m_resourceId, Logger::MediumVerbosity);
    clear();

    if (ResourcesRequest *r = request()) {
        if (m_query.isEmpty()) {
            r->list(Resources::ARTIST, m_resourceId);
        }
        else {
            r->search(Resources::ARTIST, m_query, m_order);
        }
        
        emit statusChanged(status());
    }
}

void PluginArtistModel::append(PluginArtist *artist) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    connect(artist, SIGNAL(changed()), this, SLOT(onItemChanged()));
    m_items << artist;
    endInsertRows();
}

void PluginArtistModel::insert(int row, PluginArtist *artist) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        connect(artist, SIGNAL(changed()), this, SLOT(onItemChanged()));
        m_items.insert(row, artist);
        endInsertRows();
    }
    else {
        append(artist);
    }
}

void PluginArtistModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

ResourcesRequest* PluginArtistModel::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginArtistModel::onItemChanged() {
    const int row = m_items.indexOf(qobject_cast<PluginArtist*>(sender()));
    
    if (row != -1) {
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

void PluginArtistModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_next = result.value("next").toString();
            const QVariantList list = result.value("items").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (const QVariant &item, list) {
                PluginArtist *artist = new PluginArtist(service(), item.toMap(), this);
                connect(artist, SIGNAL(changed()), this, SLOT(onItemChanged()));
                m_items << artist;
            }

            endInsertRows();
            emit countChanged(rowCount());
                
        }
    }
    else {
        Logger::log("PluginArtistModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}
