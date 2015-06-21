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

#include "resources.h"
#include "resourcesplugins.h"
#include "soundcloud.h"
#include "utils.h"
#ifdef MUSIKLOUD_DEBUG
#include <QDebug>
#endif

const QString Resources::SOUNDCLOUD("soundcloud");

const QString Resources::ARTIST("artist");
const QString Resources::CATEGORY("category");
const QString Resources::COMMENT("comment");
const QString Resources::PLAYLIST("playlist");
const QString Resources::STREAM("stream");
const QString Resources::TRACK("track");

static const QRegExp SOUNDCLOUD_REGEXP("http(s|)://(api\\.|)soundcloud\\.com/[\\w-_/]+");

ListResource::ListResource(const QString &name, const QString &type, const QString &id) :
    QVariantMap()
{
    insert("name", name);
    insert("type", type);
    insert("id", id);
}

SearchResource::SearchResource(const QString &name, const QString &type, const QString &order) :
    QVariantMap()
{
    insert("name", name);
    insert("type", type);
    insert("order", order);
}

Resources::Resources(QObject *parent) :
    QObject(parent)
{
}

QString Resources::soundcloudConstant() {
    return SOUNDCLOUD;
}

QString Resources::artistConstant() {
    return ARTIST;
}

QString Resources::categoryConstant() {
    return CATEGORY;
}

QString Resources::commentConstant() {
    return COMMENT;
}

QString Resources::playlistConstant() {
    return PLAYLIST;
}

QString Resources::streamConstant() {
    return STREAM;
}

QString Resources::trackConstant() {
    return TRACK;
}

QVariantMap Resources::getResourceFromUrl(QString url) {
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "Resources::getResourceFromUrl" << url;
#endif
    url = Utils::unescape(url);
    QVariantMap result;

    if (SOUNDCLOUD_REGEXP.indexIn(url) == 0) {
        result.insert("service", SOUNDCLOUD);
        result.insert("id", url);

        if ((url.contains("/sets/")) || (url.contains("/playlists/"))) {
            result.insert("type", PLAYLIST);
        }
        else if ((url.contains("/tracks/")) || (url.count('/') > 3)) {
            result.insert("type", TRACK);
        }
        else {
            result.insert("type", ARTIST);
        }
    }
    else {
        QList<ResourcesPlugin> plugins = ResourcesPlugins::instance()->plugins();
        
        for (int i = 0; i < plugins.size(); i++) {
            if (!plugins.at(i).regExps.isEmpty()) {
                QMapIterator<QString, QRegExp> iterator(plugins.at(i).regExps);
                
                while (iterator.hasNext()) {
                    iterator.next();
                    
                    if (iterator.value().indexIn(url) == 0) {
                        result.insert("service", plugins.at(i).name);
                        result.insert("type", iterator.key());
                        result.insert("id", url);
#ifdef MUSIKLOUD_DEBUG
                        qDebug() << result;
#endif
                        return result;
                    }
                }
            }
        }
    }
#ifdef MUSIKLOUD_DEBUG
    qDebug() << result;
#endif
    return result;
}
