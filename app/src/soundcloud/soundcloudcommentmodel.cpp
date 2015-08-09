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
#include "soundcloud.h"
#include <qsoundcloud/urls.h>
#ifdef MUSIKLOUD_DEBUG
#include <QDebug>
#endif

SoundCloudCommentModel::SoundCloudCommentModel(QObject *parent) :
    QAbstractListModel(parent),
    m_request(new QSoundCloud::ResourcesRequest(this))
{
    m_roles[ArtistRole] = "artist";
    m_roles[ArtistIdRole] = "artistId";
    m_roles[BodyRole] = "body";
    m_roles[DateRole] = "date";
    m_roles[IdRole] = "id";
    m_roles[ThumbnailUrlRole] = "thumbnailUrl";
    m_roles[TrackIdRole] = "trackId";
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

int SoundCloudCommentModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

bool SoundCloudCommentModel::canFetchMore(const QModelIndex &) const {
    return (status() != QSoundCloud::ResourcesRequest::Loading) && (!m_nextHref.isEmpty());
}

void SoundCloudCommentModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }

    m_request->get(m_nextHref);
    emit statusChanged(status());
}

QVariant SoundCloudCommentModel::data(const QModelIndex &index, int role) const {
    if (SoundCloudComment *user = get(index.row())) {
        return user->property(m_roles[role]);
    }
    
    return QVariant();
}

QMap<int, QVariant> SoundCloudCommentModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    
    if (SoundCloudComment *user = get(index.row())) {
        QHashIterator<int, QByteArray> iterator(m_roles);
        
        while (iterator.hasNext()) {
            iterator.next();
            map[iterator.key()] = user->property(iterator.value());
        }
    }
    
    return map;
}

QVariant SoundCloudCommentModel::data(int row, const QByteArray &role) const {
    if (SoundCloudComment *user = get(row)) {
        return user->property(role);
    }
    
    return QVariant();
}

QVariantMap SoundCloudCommentModel::itemData(int row) const {
    QVariantMap map;
    
    if (SoundCloudComment *user = get(row)) {
        foreach (QByteArray role, m_roles.values()) {
            map[role] = user->property(role);
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
    clear();
    m_request->get(m_resourcePath,  m_filters);
    emit statusChanged(status());
}

void SoundCloudCommentModel::append(SoundCloudComment *user) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << user;
    endInsertRows();
}

void SoundCloudCommentModel::insert(int row, SoundCloudComment *user) {
    if ((row >= 0) && (row < m_items.size())) {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, user);
        endInsertRows();
    }
    else {
        append(user);
    }
}

void SoundCloudCommentModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.takeAt(row)->deleteLater();
        endRemoveRows();
    }
}

void SoundCloudCommentModel::onRequestFinished() {
    if (m_request->status() == QSoundCloud::ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            m_nextHref = result.value("next_href").toString().section(QSoundCloud::API_URL, -1);
            QVariantList list = result.value("collection").toList();

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + list.size() - 1);
    
            foreach (QVariant item, list) {
                m_items << new SoundCloudComment(item.toMap(), this);
            }

            endInsertRows();
            emit countChanged(rowCount());
        }
    }
    
    emit statusChanged(status());
}

void SoundCloudCommentModel::onCommentAdded(SoundCloudComment *comment) {
    if (comment->trackId() == m_resourcePath.section('/', 1, 1)) {
        insert(0, new SoundCloudComment(comment, this));
    }
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "SoundCloudCommentModel::onCommentAdded" << comment->trackId();
#endif
}
