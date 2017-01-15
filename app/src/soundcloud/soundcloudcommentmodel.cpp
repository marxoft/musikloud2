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

#include "soundcloudcommentmodel.h"
#include "logger.h"
#include "soundcloud.h"
#include <qsoundcloud/urls.h>

SoundCloudCommentModel::SoundCloudCommentModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QSoundCloud::ResourcesRequest(this))
{
    m_roles[ArtistRole] = "artist";
    m_roles[ArtistIdRole] = "artistId";
    m_roles[BodyRole] = "body";
    m_roles[DateRole] = "date";
    m_roles[ErrorStringRole] = "errorString";
    m_roles[IdRole] = "id";
    m_roles[StatusRole] = "status";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TrackIdRole] = "trackId";
    m_roles[UrlRole] = "url";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    m_request->setClientId(SoundCloud::clientId());
    m_request->setClientSecret(SoundCloud::clientSecret());
    m_request->setAccessToken(SoundCloud::accessToken());
    m_request->setRefreshToken(SoundCloud::refreshToken());
    
    connect(m_request, SIGNAL(accessTokenChanged(QString)), SoundCloud::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(refreshTokenChanged(QString)), SoundCloud::instance(), SLOT(setRefreshToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
    connect(SoundCloud::instance(), SIGNAL(commentAdded(SoundCloudComment*)),
            this, SLOT(onCommentAdded(SoundCloudComment*)));
}

QString SoundCloudCommentModel::errorString() const {
    return SoundCloud::getErrorString(m_request->result().toMap());
}

QSoundCloud::ResourcesRequest::Status SoundCloudCommentModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> SoundCloudCommentModel::roleNames() const {
    return m_roles;
}
#endif

int SoundCloudCommentModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_items.size();
}

int SoundCloudCommentModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 3;
}

bool SoundCloudCommentModel::canFetchMore(const QModelIndex &parent) const {
    return (!parent.isValid()) && (status() != QSoundCloud::ResourcesRequest::Loading) && (!m_nextHref.isEmpty());
}

void SoundCloudCommentModel::fetchMore(const QModelIndex &parent) {
    if (!canFetchMore(parent)) {
        return;
    }

    m_request->get(m_nextHref);
    emit statusChanged(status());
}

QVariant SoundCloudCommentModel::headerData(int section, Qt::Orientation orientation, int role) const {
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

QVariant SoundCloudCommentModel::data(const QModelIndex &index, int role) const {
    if (const SoundCloudComment *comment = get(index.row())) {
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

QMap<int, QVariant> SoundCloudCommentModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (const SoundCloudComment *comment = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = comment->property(iterator.value());
        }
    }
    
    return map;
}

QVariant SoundCloudCommentModel::data(int row, const QByteArray &role) const {
    if (const SoundCloudComment *comment = get(row)) {
        return comment->property(role);
    }
    
    return QVariant();
}

QVariantMap SoundCloudCommentModel::itemData(int row) const {
    QVariantMap map;
    
    if (const SoundCloudComment *comment = get(row)) {
        foreach (const QByteArray &role, m_roles.values()) {
            map[role] = comment->property(role);
        }
    }
    
    return map;
}

SoundCloudComment* SoundCloudCommentModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void SoundCloudCommentModel::get(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("SoundCloudCommentModel::get(). Resource path: " + resourcePath, Logger::HighVerbosity);
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_filters["linked_partitioning"] = true;
    m_request->get(m_resourcePath, m_filters);
    emit statusChanged(status());    
}

void SoundCloudCommentModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_nextHref = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void SoundCloudCommentModel::cancel() {
    m_request->cancel();
}

void SoundCloudCommentModel::reload() {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("SoundCloudCommentModel::reload(). Resource path: " + m_resourcePath, Logger::HighVerbosity);
    clear();
    m_request->get(m_resourcePath,  m_filters);
    emit statusChanged(status());
}

void SoundCloudCommentModel::append(SoundCloudComment *comment) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    connect(comment, SIGNAL(changed()), this, SLOT(onItemChanged()));
    m_items << comment;
    endInsertRows();
}

void SoundCloudCommentModel::insert(int row, SoundCloudComment *comment) {
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

void SoundCloudCommentModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void SoundCloudCommentModel::onItemChanged() {
    const int row = m_items.indexOf(qobject_cast<SoundCloudComment*>(sender()));
    
    if (row != -1) {
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

void SoundCloudCommentModel::onRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_nextHref = result.value("next_href").toString().section(QSoundCloud::API_URL, -1);
            const QVariantList list = result.value("collection").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (const QVariant &item, list) {
                SoundCloudComment *comment = new SoundCloudComment(item.toMap(), this);
                connect(comment, SIGNAL(changed()), this, SLOT(onItemChanged()));
                m_items << comment;
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    else {
        Logger::log("SoundCloudCommentModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}

void SoundCloudCommentModel::onCommentAdded(SoundCloudComment *comment) {
    if (comment->trackId() == m_resourcePath.section('/', 1, 1)) {
        insert(0, new SoundCloudComment(comment, this));
    }
}
