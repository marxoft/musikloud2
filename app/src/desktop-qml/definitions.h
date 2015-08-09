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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QRegExp>
#include <QStringList>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

static const int MAX_CONCURRENT_TRANSFERS = 4;
static const int MAX_REDIRECTS = 8;

static const int MAX_RESULTS = 20;

static const int LARGE_THUMBNAIL_SIZE = 300;
static const int THUMBNAIL_SIZE = 64;

static const QRegExp ILLEGAL_FILENAME_CHARS_RE("[\"@&~=\\/:?#!|<>*^]");

static const QString VERSION_NUMBER("0.0.3");

static const QStringList SUPPORTED_AUDIO_FORMATS = QStringList() << "*.aiff" << "*.ape" << "*.flac" << "*.m4a" << "*.mp3"
                                                                 << "*.ogg" << "*.wav" << "*.wma";

#if QT_VERSION >= 0x050000
static const QString DATABASE_PATH(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/MusiKloud2/");
static const QString DOWNLOAD_PATH(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/MusiKloud2/");
static const QString STORAGE_PATH(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/MusiKloud2/");
static const QStringList PLUGIN_PATHS = QStringList() << "/opt/musikloud2/plugins/"
                                                      << QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
                                                         + "/MusiKloud2/plugins/";
#else
static const QString DATABASE_PATH(QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/.config/MusiKloud2/");
static const QString DOWNLOAD_PATH(QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/MusiKloud2/");
static const QString STORAGE_PATH(QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/.config/MusiKloud2/");
static const QStringList PLUGIN_PATHS = QStringList() << "/opt/musikloud2/plugins/"
                                                      << QDesktopServices::storageLocation(QDesktopServices::HomeLocation)
                                                         + "/.config/MusiKloud2/plugins/";
#endif

#endif // DEFINITIONS_H
