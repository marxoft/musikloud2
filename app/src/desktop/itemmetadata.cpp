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

#include "itemmetadata.h"
#include "image.h"
#include <QHBoxLayout>
#include <QLabel>

ItemMetaDataValue::ItemMetaDataValue(const QString &name, const QVariant &value)
{
    this->name = name;
    this->value = value;
}

ItemMetaData::ItemMetaData(QWidget *parent) :
    QWidget(parent),
    m_image(new Image(this)),
    m_label(new QLabel(this)),
    m_layout(new QHBoxLayout(this))
{
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    
    m_layout->addWidget(m_image, Qt::AlignLeft | Qt::AlignTop);
    m_layout->addWidget(m_label, Qt::AlignLeft | Qt::AlignTop);
}

QList<ItemMetaDataValue> ItemMetaData::metaData() const {
    return m_metaData;
}

void ItemMetaData::setMetaData(const QList<ItemMetaDataValue> &metaData) {
    m_metaData = metaData;
    QString text;
    
    for (int i = 0; i < metaData.size(); i++) {
        text.append(metaData.at(i).name + ": " + metaData.at(i).value.toString() + "\n");
    }
    
    m_label->setText(text);
}

QSize ItemMetaData::thumbnailSize() const {
    return m_image->size();
}

void ItemMetaData::setThumbnailSize(const QSize &size) {
    m_image->setFixedSize(size);
}

QUrl ItemMetaData::thumbnailSource() const {
    return m_image->source();
}

void ItemMetaData::setThumbnailSource(const QUrl &url) {
    m_image->setSource(url);
    m_image->setVisible(!url.isEmpty());
}

void ItemMetaData::clear() {
    m_image->setSource(QUrl());
    m_image->hide();
    m_label->clear();
}
