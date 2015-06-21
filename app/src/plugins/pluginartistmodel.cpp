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
#include "resources.h"

PluginArtistModel::PluginArtistModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new ResourcesRequest(this))
{
    m_roles[DescriptionRole] = "description";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[NameRole] = "name";
    m_roles[ServiceRole] = "service";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    connect(m_request, SIGNAL(serviceChanged()), this, SIGNAL(serviceChanged()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString PluginArtistModel::service() const {
    return m_request->service();
}

void PluginArtistModel::setService(const QString &s) {
    m_request->setService(s);
}

QString PluginArtistModel::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginArtistModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> PluginArtistModel::roleNames() const {
    return m_roles;
}
#endif

int PluginArtistModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool PluginArtistModel::canFetchMore(const QModelIndex &) const {
    return (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginArtistModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    m_request->list(Resources::ARTIST, m_next);
    emit statusChanged(status());
}

QVariant PluginArtistModel::data(const QModelIndex &index, int role) const {
    if (PluginArtist *artist = get(index.row())) {
        return artist->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> PluginArtistModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (PluginArtist *artist = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = artist->property(iterator.value());
        }
    }
    
    return map;
}

QVariant PluginArtistModel::data(int row, const QByteArray &role) const {
    if (PluginArtist *artist = get(row)) {
        return artist->property(role);
    }
    
    return QVariant();
}

QVariantMap PluginArtistModel::itemData(int row) const {
    QVariantMap map;
    
    if (PluginArtist *artist = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
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

void PluginArtistModel::list(const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_query = QString();
    m_request->list(Resources::ARTIST, id);
    emit statusChanged(status());
}

void PluginArtistModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = QString();
    m_query = query;
    m_order = order;
    m_request->search(Resources::ARTIST, query, order);
    emit statusChanged(status());
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
    clear();
    
    if (m_query.isEmpty()) {
        m_request->list(Resources::ARTIST, m_id);
    }
    else {
        m_request->search(Resources::ARTIST, m_query, m_order);
    }
    
    emit statusChanged(status());
}

void PluginArtistModel::append(PluginArtist *artist) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << artist;
    endInsertRows();
}

void PluginArtistModel::insert(int row, PluginArtist *artist) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
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

void PluginArtistModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_next = result.value("next").toString();
            QVariantList list = result.value("items").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new PluginArtist(service(), item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
                
        }
    }
    
    emit statusChanged(status());
}
