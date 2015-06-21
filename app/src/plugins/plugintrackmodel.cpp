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

#include "plugintrackmodel.h"
#include "resources.h"

PluginTrackModel::PluginTrackModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new ResourcesRequest(this))
{
    m_roles[ArtistRole] = "artist";
    m_roles[ArtistIdRole] = "artistId";
    m_roles[DateRole] = "date";
    m_roles[DescriptionRole] = "description";
    m_roles[DurationRole] = "duration";
    m_roles[DurationStringRole] = "durationString";
    m_roles[FormatRole] = "format";
    m_roles[GenreRole] = "genre";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[PlayCountRole] = "playCount";
    m_roles[ServiceRole] = "service";
    m_roles[SizeRole] = "size";
    m_roles[SizeStringRole] = "sizeString";
    m_roles[StreamUrlRole] = "streamUrl";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
    m_roles[UrlRole] = "url";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    connect(m_request, SIGNAL(serviceChanged()), this, SIGNAL(serviceChanged()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString PluginTrackModel::service() const {
    return m_request->service();
}

void PluginTrackModel::setService(const QString &s) {
    m_request->setService(s);
}

QString PluginTrackModel::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginTrackModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> PluginTrackModel::roleNames() const {
    return m_roles;
}
#endif

int PluginTrackModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool PluginTrackModel::canFetchMore(const QModelIndex &) const {
    return (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginTrackModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    m_request->list(Resources::TRACK, m_next);
    emit statusChanged(status());
}

QVariant PluginTrackModel::data(const QModelIndex &index, int role) const {
    if (PluginTrack *track = get(index.row())) {
        return track->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> PluginTrackModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (PluginTrack *track = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = track->property(iterator.value());
        }
    }
    
    return map;
}

QVariant PluginTrackModel::data(int row, const QByteArray &role) const {
    if (PluginTrack *track = get(row)) {
        return track->property(role);
    }
    
    return QVariant();
}

QVariantMap PluginTrackModel::itemData(int row) const {
    QVariantMap map;
    
    if (PluginTrack *track = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = track->property(role);
        }
    }
    
    return map;
}

PluginTrack* PluginTrackModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void PluginTrackModel::list(const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_query = QString();
    m_request->list(Resources::TRACK, id);
    emit statusChanged(status());
}

void PluginTrackModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = QString();
    m_query = query;
    m_order = order;
    m_request->search(Resources::TRACK, query, order);
    emit statusChanged(status());
}

void PluginTrackModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_next = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void PluginTrackModel::cancel() {
    m_request->cancel();
}

void PluginTrackModel::reload() {
    clear();
    
    if (m_query.isEmpty()) {
        m_request->list(Resources::TRACK, m_id);
    }
    else {
        m_request->search(Resources::TRACK, m_query, m_order);
    }
    
    emit statusChanged(status());
}

void PluginTrackModel::append(PluginTrack *track) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << track;
    endInsertRows();
}

void PluginTrackModel::insert(int row, PluginTrack *track) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, track);
        endInsertRows();
    }
    else {
        append(track);
    }
}

void PluginTrackModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void PluginTrackModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_next = result.value("next").toString();
            QVariantList list = result.value("items").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new PluginTrack(service(), item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
                
        }
    }
    
    emit statusChanged(status());
}
