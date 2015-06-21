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

#ifndef PLUGINARTIST_H
#define PLUGINARTIST_H

#include "artist.h"
#include "resourcesrequest.h"

class PluginArtist : public MKArtist
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(ResourcesRequest::Status status READ status NOTIFY statusChanged)

public:
    explicit PluginArtist(QObject *parent = 0);
    explicit PluginArtist(const QString &service, const QString &id, QObject *parent = 0);
    explicit PluginArtist(const QString &service, const QVariantMap &artist, QObject *parent = 0);
    explicit PluginArtist(PluginArtist *artist, QObject *parent = 0);
    
    QString errorString() const;
        
    ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadArtist(const QString &service, const QString &id);
    Q_INVOKABLE void loadArtist(const QString &service, const QVariantMap &artist);
    Q_INVOKABLE void loadArtist(PluginArtist *artist);
            
private Q_SLOTS:
    void onRequestFinished();
    
Q_SIGNALS:
    void statusChanged(ResourcesRequest::Status s);

private:
    ResourcesRequest *m_request;
};

#endif // PLUGINARTIST_H
