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

#include "soundcloudactivitymodel.h"
#include "soundcloud.h"

SoundCloudActivityModel::SoundCloudActivityModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QSoundCloud::ResourcesRequest(this))
{
    m_roles[ActivityTypeRole] = "activityType";
    m_roles[ActivityTypeStringRole] = "activityTypeString";
    m_roles[ArtistRole] = "artist";
    m_roles[ArtistIdRole] = "artistId";
    m_roles[DateRole] = "date";
    m_roles[DescriptionRole] = "description";
    m_roles[IdRole] = "id";
    m_roles[OriginIdRole] = "originId";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TitleRole] = "title";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    m_request->setClientId(SoundCloud::instance()->clientId());
    m_request->setClientSecret(SoundCloud::instance()->clientSecret());
    m_request->setAccessToken(SoundCloud::instance()->accessToken());
    m_request->setRefreshToken(SoundCloud::instance()->refreshToken());
        
    connect(m_request, SIGNAL(accessTokenChanged(QString)), SoundCloud::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(refreshTokenChanged(QString)), SoundCloud::instance(), SLOT(setRefreshToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString SoundCloudActivityModel::errorString() const {
    return SoundCloud::getErrorString(m_request->result().toMap());
}

QSoundCloud::ResourcesRequest::Status SoundCloudActivityModel::status() const {
    return m_request->status();
}

#if QT_VERSION >=0x050000
QHash<int, QByteArray> SoundCloudActivityModel::roleNames() const {
    return m_roles;
}
#endif

int SoundCloudActivityModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool SoundCloudActivityModel::canFetchMore(const QModelIndex &) const {
    return (status() != QSoundCloud::ResourcesRequest::Loading) && (!m_nextHref.isEmpty());
}

void SoundCloudActivityModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    m_request->get(m_nextHref);
    emit statusChanged(status());
}

QVariant SoundCloudActivityModel::data(const QModelIndex &index, int role) const {
    if (SoundCloudActivity *activity = get(index.row())) {
        return activity->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> SoundCloudActivityModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (SoundCloudActivity *activity = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = activity->property(iterator.value());
        }
    }
    
    return map;
}

QVariant SoundCloudActivityModel::data(int row, const QByteArray &role) const {
    if (SoundCloudActivity *activity = get(row)) {
        return activity->property(role);
    }
    
    return QVariant();
}

QVariantMap SoundCloudActivityModel::itemData(int row) const {
    QVariantMap map;
    
    if (SoundCloudActivity *activity = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = activity->property(role);
        }
    }
    
    return map;
}

SoundCloudActivity* SoundCloudActivityModel::get(int row) const {
    if ((row >= 0) && (row < m_items.size())) {
        return m_items.at(row);
    }
    
    return 0;
}

void SoundCloudActivityModel::get(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QSoundCloud::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_filters["linked_partitioning"] = true;
    m_request->get(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void SoundCloudActivityModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        m_nextHref = QString();
        endResetModel();
        emit countChanged(rowCount());
    }
}

void SoundCloudActivityModel::cancel() {
    m_request->cancel();
}

void SoundCloudActivityModel::reload() {
    clear();
    m_request->get(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void SoundCloudActivityModel::append(SoundCloudActivity *activity) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << activity;
    endInsertRows();
}

void SoundCloudActivityModel::insert(int row, SoundCloudActivity *activity) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, activity);
        endInsertRows();
    }
    else {
        append(activity);
    }
}

void SoundCloudActivityModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void SoundCloudActivityModel::onRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_nextHref = result.value("next_href").toString().section('/', -1);
            QVariantList list = result.value("collection").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new SoundCloudActivity(item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    
    emit statusChanged(status());
}
