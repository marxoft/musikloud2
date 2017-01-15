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

#include "soundcloudnavmodel.h"
#include "soundcloud.h"

SoundCloudNavModel::SoundCloudNavModel(QObject *parent) :
    QStringListModel(parent)
{
    reload();
    connect(SoundCloud::instance(), SIGNAL(userIdChanged(QString)), this, SLOT(reload()));
}

void SoundCloudNavModel::reload() {
    if (SoundCloud::userId().isEmpty()) {
        setStringList(QStringList() << tr("Accounts") << tr("Search"));
    }
    else {
        setStringList(QStringList() << tr("Accounts") << tr("Search") << tr("Tracks") << tr("Favourites") << tr("Sets")
                                    << tr("Followings"));
    }
}
