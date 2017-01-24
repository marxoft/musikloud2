/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QRegExp>
#include <QStringList>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

// Home
#if QT_VERSION >= 0x050000
static const QString HOME_PATH(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
#else
static const QString HOME_PATH(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
#endif

// Plugins
static const QStringList PLUGIN_PATHS = QStringList() << QString("/usr/share/musikloud2/plugins/")
                                                      << QString(HOME_PATH + "/musikloud2/plugins/");

static const QString LIB_PREFIX("lib");
static const QString LIB_SUFFIX(".so");

// Config
static const QString APP_CONFIG_PATH(HOME_PATH + "/.config/MusiKloud2/");
static const QString PLUGIN_CONFIG_PATH(APP_CONFIG_PATH + "plugins/");

// Downloads
static const QString DOWNLOAD_PATH(HOME_PATH + "/Downloads/musikloud2/");
static const QRegExp ILLEGAL_FILENAME_CHARS_RE("[\"@&~=\\/:?#!|<>*^]");

// Content
static const int LARGE_THUMBNAIL_SIZE = 120;
static const int THUMBNAIL_SIZE = 40;

// Network
static const int DOWNLOAD_BUFFER_SIZE = 64000;
static const int MAX_CONCURRENT_TRANSFERS = 4;
static const int MAX_REDIRECTS = 8;
static const int MAX_RESULTS = 20;
static const QByteArray USER_AGENT("Wget/1.13.4 (linux-gnu)");

// Version
static const QString VERSION_NUMBER("0.1.0");

#endif // DEFINITIONS_H
