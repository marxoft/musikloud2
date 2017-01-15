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

#ifndef ITEMMETADATA_H
#define ITEMMETADATA_H

#include <QWidget>
#include <QList>
#include <QUrl>
#include <QVariant>

class Image;
class QHBoxLayout;
class QLabel;

class ItemMetaDataValue
{

public:
    explicit ItemMetaDataValue(const QString &name, const QVariant &value);
    
    QString name;
    QVariant value;
};

class ItemMetaData : public QWidget
{
    Q_OBJECT
    
    Q_PROPERTY(QSize thumbnailSize READ thumbnailSize WRITE setThumbnailSize)
    Q_PROPERTY(QUrl thumbnailSource READ thumbnailSource WRITE setThumbnailSource)

public:
    explicit ItemMetaData(QWidget *parent = 0);
    
    QList<ItemMetaDataValue> metaData() const;
    void setMetaData(const QList<ItemMetaDataValue> &metaData);
    
    QSize thumbnailSize() const;
    void setThumbnailSize(const QSize &size);
    
    QUrl thumbnailSource() const;
    void setThumbnailSource(const QUrl &url);

public Q_SLOTS:
    void clear();

private:
    Image *m_image;
    
    QLabel *m_label;
    
    QHBoxLayout *m_layout;
    
    QList<ItemMetaDataValue> m_metaData;    
};

#endif // ITEMMETADATA_H
