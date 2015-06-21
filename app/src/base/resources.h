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

#ifndef RESOURCES_H
#define RESOURCES_H

#include <QObject>
#include <QVariantMap>

class ListResource : public QVariantMap
{

public:
    ListResource(const QString &name, const QString &type, const QString &id);
};

class SearchResource : public QVariantMap
{

public:
    SearchResource(const QString &name, const QString &type, const QString &order);
};

class Resources : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString SOUNDCLOUD READ soundcloudConstant CONSTANT)    
    
    Q_PROPERTY(QString ARTIST READ artistConstant CONSTANT)
    Q_PROPERTY(QString CATEGORY READ categoryConstant CONSTANT)
    Q_PROPERTY(QString COMMENT READ commentConstant CONSTANT)
    Q_PROPERTY(QString PLAYLIST READ playlistConstant CONSTANT)
    Q_PROPERTY(QString STREAM READ streamConstant CONSTANT)
    Q_PROPERTY(QString TRACK READ trackConstant CONSTANT)
        
public:
    explicit Resources(QObject *parent = 0);
    
    static const QString SOUNDCLOUD;
    
    static const QString ARTIST;
    static const QString CATEGORY;
    static const QString COMMENT;
    static const QString PLAYLIST;
    static const QString STREAM;
    static const QString TRACK;
        
    static QString soundcloudConstant();
    
    static QString artistConstant();
    static QString categoryConstant();
    static QString commentConstant();
    static QString playlistConstant();
    static QString streamConstant();
    static QString trackConstant();
    
    Q_INVOKABLE static QVariantMap getResourceFromUrl(QString url);    
};

#endif // RESOURCES_H
