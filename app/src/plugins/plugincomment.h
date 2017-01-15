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

#ifndef PLUGINCOMMENT_H
#define PLUGINCOMMENT_H

#include "comment.h"
#include "resourcesrequest.h"

class PluginComment : public MKComment
{
    Q_OBJECT
    
    Q_PROPERTY(QVariantList actions READ actions NOTIFY actionsChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(ResourcesRequest::Status status READ status NOTIFY statusChanged)

public:
    explicit PluginComment(QObject *parent = 0);
    explicit PluginComment(const QString &service, const QString &id, QObject *parent = 0);
    explicit PluginComment(const QString &service, const QVariantMap &comment, QObject *parent = 0);
    explicit PluginComment(const PluginComment *comment, QObject *parent = 0);
    
    QVariantList actions() const;
    
    QString errorString() const;
        
    ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadComment(const QString &service, const QString &id);
    Q_INVOKABLE void loadComment(const QString &service, const QVariantMap &comment);
    Q_INVOKABLE void loadComment(PluginComment *comment);

public Q_SLOTS:
    void cancel();
    void del(const QString &resourceType, const QString &resourceId);
    void insert(const QString &resourceType, const QString &resourceId);

protected:
    void setActions(const QVariantList &a);

private Q_SLOTS:
    void onRequestFinished();
    
Q_SIGNALS:
    void actionsChanged();
    void statusChanged(ResourcesRequest::Status s);

private:
    ResourcesRequest* request();
    
    ResourcesRequest *m_request;
    
    QVariantList m_actions;
};

#endif // PLUGINCOMMENT_H
