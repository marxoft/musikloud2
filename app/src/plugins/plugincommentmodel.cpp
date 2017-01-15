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

#include "plugincommentmodel.h"
#include "logger.h"
#include "pluginmanager.h"
#include "resources.h"

PluginCommentModel::PluginCommentModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(0)
{
    m_roles[ActionsRole] = "actions";
    m_roles[ArtistRole] = "artist";
    m_roles[ArtistIdRole] = "artistId";
    m_roles[BodyRole] = "body";
    m_roles[DateRole] = "date";
    m_roles[ErrorStringRole] = "errorString";
    m_roles[IdRole] = "id";
    m_roles[ServiceRole] = "service";
    m_roles[StatusRole] = "status";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TrackIdRole] = "trackId";
    m_roles[UrlRole] = "url";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
}

QString PluginCommentModel::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

QString PluginCommentModel::service() const {
    return m_service;
}

void PluginCommentModel::setService(const QString &s) {
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

ResourcesRequest::Status PluginCommentModel::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> PluginCommentModel::roleNames() const {
    return m_roles;
}
#endif

int PluginCommentModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_items.size();
}

int PluginCommentModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 3;
}

bool PluginCommentModel::canFetchMore(const QModelIndex &parent) const {
    return (!parent.isValid()) && (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginCommentModel::fetchMore(const QModelIndex &parent) {
    if (!canFetchMore(parent)) {
        return;
    }

    if (ResourcesRequest *r = request()) {
        r->list(Resources::COMMENT, m_next);
        emit statusChanged(status());
    }
}

QVariant PluginCommentModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((orientation != Qt::Horizontal) || (role != Qt::DisplayRole)) {
        return QVariant();
    }
    
    switch (section) {
    case 0:
        return tr("Artist");
    case 1:
        return tr("Date");
    case 2:
        return tr("Comment");
    default:
        return QVariant();
    }
}

QVariant PluginCommentModel::data(const QModelIndex &index, int role) const {
    if (const PluginComment *comment = get(index.row())) {
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case 0:
                return comment->artist();
            case 1:
                return comment->date();
            case 2:
                return comment->body();
            default:
                return QVariant();
            }
        }
        
        return comment->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> PluginCommentModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const PluginComment *comment = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = comment->property(iterator.value());
        }
    }
    
    return map;
}

QVariant PluginCommentModel::data(int row, const QByteArray &role) const {
    if (const PluginComment *comment = get(row)) {        
        return comment->property(role);
    }
    
    return QVariant();
}

QVariantMap PluginCommentModel::itemData(int row) const {
    QVariantMap map;
    
    if (const PluginComment *comment = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
            map[role] = comment->property(role);
        }
    }
    
    return map;
}

PluginComment* PluginCommentModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void PluginCommentModel::list(const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginCommentModel::list(). Resource ID: " + resourceId, Logger::MediumVerbosity);
    clear();
    m_resourceId = resourceId;
    m_query = QString();

    if (ResourcesRequest *r = request()) {
        r->list(Resources::COMMENT, resourceId);
        emit statusChanged(status());
    }
}

void PluginCommentModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log(QString("PluginCommentModel::search(). Query: %1, Order: %2").arg(query).arg(order),
                Logger::MediumVerbosity);
    clear();
    m_resourceId = QString();
    m_query = query;
    m_order = order;

    if (ResourcesRequest *r = request()) {
        r->search(Resources::COMMENT, query, order);
        emit statusChanged(status());
    }
}

void PluginCommentModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_next = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void PluginCommentModel::cancel() {
    if (m_request) {
        m_request->cancel();
    }
}

void PluginCommentModel::reload() {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("PluginCommentModel::reload(). Resource ID: " + m_resourceId, Logger::MediumVerbosity);
    clear();

    if (ResourcesRequest *r = request()) {
        if (m_query.isEmpty()) {
            r->list(Resources::COMMENT, m_resourceId);
        }
        else {
            r->search(Resources::COMMENT, m_query, m_order);
        }
        
        emit statusChanged(status());
    }
}

void PluginCommentModel::append(PluginComment *comment) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    connect(comment, SIGNAL(changed()), this, SLOT(onItemChanged()));
    m_items << comment;
    endInsertRows();
}

void PluginCommentModel::insert(int row, PluginComment *comment) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        connect(comment, SIGNAL(changed()), this, SLOT(onItemChanged()));
        m_items.insert(row, comment);
        endInsertRows();
    }
    else {
        append(comment);
    }
}

void PluginCommentModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

ResourcesRequest* PluginCommentModel::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginCommentModel::onItemChanged() {
    const int row = m_items.indexOf(qobject_cast<PluginComment*>(sender()));
    
    if (row != -1) {
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

void PluginCommentModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_next = result.value("next").toString();
            const QVariantList list = result.value("items").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (const QVariant &item, list) {
                PluginComment *comment = new PluginComment(service(), item.toMap(), this);
                connect(comment, SIGNAL(changed()), this, SLOT(onItemChanged()));
                m_items << comment;
            }

            endInsertRows();
            emit countChanged(rowCount());
                
        }
    }
    else {
        Logger::log("PluginCommentModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}
