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

#include "plugincomment.h"
#include "pluginmanager.h"
#include "resources.h"

PluginComment::PluginComment(QObject *parent) :
    MKComment(parent),
    m_request(0)
{
}

PluginComment::PluginComment(const QString &service, const QString &id, QObject *parent) :
    MKComment(parent),
    m_request(0)
{
    loadComment(service, id);
}

PluginComment::PluginComment(const QString &service, const QVariantMap &comment, QObject *parent) :
    MKComment(parent),
    m_request(0)
{
    loadComment(service, comment);
}

PluginComment::PluginComment(const PluginComment *comment, QObject *parent) :
    MKComment(comment, parent),
    m_request(0),
    m_actions(comment->actions())
{
}

QVariantList PluginComment::actions() const {
    return m_actions;
}

void PluginComment::setActions(const QVariantList &a) {
    m_actions = a;
    emit changed();
    emit actionsChanged();
}

QString PluginComment::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

ResourcesRequest::Status PluginComment::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

void PluginComment::loadComment(const QString &service, const QString &id) {
    setService(service);
    setId(id);
    
    if (status() == ResourcesRequest::Loading) {
        return;
    }

    if (ResourcesRequest *r = request()) {
        r->get(Resources::COMMENT, id);
        emit changed();
        emit statusChanged(status());
    }
}

void PluginComment::loadComment(const QString &service, const QVariantMap &comment) {
    setService(service);
    
    if (comment.contains("actions")) {
        setActions(comment.value("actions").toList());
    }
    
    if (comment.contains("artist")) {
        setArtist(comment.value("artist").toString());
    }
    
    if (comment.contains("artistId")) {
        setArtistId(comment.value("artistId").toString());
    }
    
    if (comment.contains("body")) {
        setBody(comment.value("body").toString());
    }
    
    if (comment.contains("date")) {
        setDate(comment.value("date").toString());
    }
    
    if (comment.contains("id")) {
        setId(comment.value("id").toString());
    }
    
    if (comment.contains("thumbnailUrl")) {
        setThumbnailUrl(comment.value("thumbnailUrl").toString());
    }
    
    if (comment.contains("trackId")) {
        setTrackId(comment.value("trackId").toString());
    }

    if (comment.contains("url")) {
        setUrl(comment.value("url").toString());
    }
}

void PluginComment::loadComment(PluginComment *comment) {
    MKComment::loadComment(comment);
    setActions(comment->actions());
}

void PluginComment::cancel() {
    if (status() == ResourcesRequest::Loading) {
        m_request->cancel();
        emit changed();
        emit statusChanged(status());
    }
}

void PluginComment::del(const QString &resourceType, const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->del(Resources::COMMENT, id(), resourceType, resourceId);
        emit changed();
        emit statusChanged(status());
    }
}

void PluginComment::insert(const QString &resourceType, const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    if (ResourcesRequest *r = request()) {
        r->insert(Resources::COMMENT, id(), resourceType, resourceId);
        emit changed();
        emit statusChanged(status());
    }
}

ResourcesRequest* PluginComment::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginComment::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        
        if (!result.isEmpty()) {
            loadComment(service(), result);
        }
    }
    
    emit changed();
    emit statusChanged(status());
}
