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
#include "logger.h"
#include "pluginmanager.h"
#include "resources.h"

PluginTrackModel::PluginTrackModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(0)
{
    m_roles[ActionsRole] = "actions";
    m_roles[ArtistRole] = "artist";
    m_roles[ArtistIdRole] = "artistId";
    m_roles[DateRole] = "date";
    m_roles[CommentsIdRole] = "commentsId";
    m_roles[DescriptionRole] = "description";
    m_roles[DownloadableRole] = "downloadable";
    m_roles[DurationRole] = "duration";
    m_roles[DurationStringRole] = "durationString";
    m_roles[ErrorStringRole] = "errorString";
    m_roles[FormatRole] = "format";
    m_roles[GenreRole] = "genre";
    m_roles[IdRole] = "id";
    m_roles[LargeThumbnailUrlRole] = "largeThumbnailUrl";
    m_roles[PlayCountRole] = "playCount";
    m_roles[RelatedTracksIdRole] = "relatedTracksId";
    m_roles[ServiceRole] = "service";
    m_roles[SizeRole] = "size";
    m_roles[SizeStringRole] = "sizeString";
    m_roles[StatusRole] = "status";
    m_roles[StreamUrlRole] = "streamUrl";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
    m_roles[UrlRole] = "url";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
}

QString PluginTrackModel::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

QString PluginTrackModel::service() const {
    return m_service;
}

void PluginTrackModel::setService(const QString &s) {
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

ResourcesRequest::Status PluginTrackModel::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> PluginTrackModel::roleNames() const {
    return m_roles;
}
#endif

int PluginTrackModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_items.size();
}

int PluginTrackModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 4;
}

bool PluginTrackModel::canFetchMore(const QModelIndex &parent) const {
    return (!parent.isValid()) && (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginTrackModel::fetchMore(const QModelIndex &parent) {
    if (!canFetchMore(parent)) {
        return;
    }

    if (ResourcesRequest *r = request()) {
        r->list(Resources::TRACK, m_next);
        emit statusChanged(status());
    }
}

QVariant PluginTrackModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((orientation != Qt::Horizontal) || (role != Qt::DisplayRole)) {
        return QVariant();
    }
    
    switch (section) {
    case 0:
        return tr("Title");
    case 1:
        return tr("Artist");
    case 2:
        return tr("Genre");
    case 3:
        return tr("Duration");
    default:
        return QVariant();
    }
}

QVariant PluginTrackModel::data(const QModelIndex &index, int role) const {
    if (const PluginTrack *track = get(index.row())) {
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case 0:
                return track->title();
            case 1:
                return track->artist();
            case 2:
                return track->genre();
            case 3:
                return track->durationString();
            default:
                return QVariant();
            }
        }
        
        return track->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> PluginTrackModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const PluginTrack *track = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = track->property(iterator.value());
        }
    }
    
    return map;
}

QVariant PluginTrackModel::data(int row, const QByteArray &role) const {
    if (const PluginTrack *track = get(row)) {
        return track->property(role);
    }
    
    return QVariant();
}

QVariantMap PluginTrackModel::itemData(int row) const {
    QVariantMap map;
    
    if (const PluginTrack *track = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
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

void PluginTrackModel::list(const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginTrackModel::list(). Resource ID: " + resourceId, Logger::MediumVerbosity);
    clear();
    m_resourceId = resourceId;
    m_query = QString();

    if (ResourcesRequest *r = request()) {
        r->list(Resources::TRACK, resourceId);
        emit statusChanged(status());
    }
}

void PluginTrackModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log(QString("PluginTrackModel::search(). Query: %1, Order: %2").arg(query).arg(order),
                Logger::MediumVerbosity);
    clear();
    m_resourceId = QString();
    m_query = query;
    m_order = order;

    if (ResourcesRequest *r = request()) {
        r->search(Resources::TRACK, query, order);
        emit statusChanged(status());
    }
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
    if (m_request) {
        m_request->cancel();
    }
}

void PluginTrackModel::reload() {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginTrackModel::reload(). Resource ID: " + m_resourceId, Logger::MediumVerbosity);
    clear();

    if (ResourcesRequest *r = request()) {
        if (m_query.isEmpty()) {
            r->list(Resources::TRACK, m_resourceId);
        }
        else {
            r->search(Resources::TRACK, m_query, m_order);
        }
        
        emit statusChanged(status());
    }
}

void PluginTrackModel::append(PluginTrack *track) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    connect(track, SIGNAL(changed()), this, SLOT(onItemChanged()));
    m_items << track;
    endInsertRows();
}

void PluginTrackModel::insert(int row, PluginTrack *track) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        connect(track, SIGNAL(changed()), this, SLOT(onItemChanged()));
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

ResourcesRequest* PluginTrackModel::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginTrackModel::onItemChanged() {
    const int row = m_items.indexOf(qobject_cast<PluginTrack*>(sender()));
    
    if (row != -1) {
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

void PluginTrackModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_next = result.value("next").toString();
            const QVariantList list = result.value("items").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (const QVariant &item, list) {
                PluginTrack *track = new PluginTrack(service(), item.toMap(), this);
                connect(track, SIGNAL(changed()), this, SLOT(onItemChanged()));
                m_items << track;
            }

            endInsertRows();
            emit countChanged(rowCount());
                
        }
    }
    else {
        Logger::log("PluginTrackModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}
