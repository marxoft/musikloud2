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

#include "searchdialog.h"
#include "pluginmanager.h"
#include "resources.h"
#include "selectionmodel.h"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

SearchDialog::SearchDialog(QWidget *parent) :
    QDialog(parent),
    m_model(new SelectionModel(this)),
    m_queryEdit(new QLineEdit(this)),
    m_serviceSelector(new QComboBox(this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)),
    m_layout(new QFormLayout(this))
{
    setWindowTitle(tr("Search"));
    
    QVariantMap resource;
    resource["service"] = Resources::SOUNDCLOUD;
    resource["type"] = Resources::TRACK;
    m_model->append(tr("SoundCloud: Tracks"), resource);
    
    const ServicePluginList plugins = PluginManager::instance()->plugins();
    
    for (int i = 0; i < plugins.size(); i++) {
        const ServicePluginConfig *config = plugins.at(i).config;
        foreach (SearchResource resource, config->searchResources()) {
            if (resource.type() == Resources::TRACK) {
                resource["service"] = config->id();
                m_model->append(QString("%1: %2").arg(config->displayName()).arg(resource.label()), resource);
            }
        }
    }
    
    m_serviceSelector->setModel(m_model);
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    
    m_layout->addRow(tr("&Query"), m_queryEdit);
    m_layout->addRow(tr("&Service"), m_serviceSelector);
    m_layout->addWidget(m_buttonBox);
    
    connect(m_queryEdit, SIGNAL(textChanged(QString)), this, SLOT(onQueryChanged(QString)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString SearchDialog::service() const {
    return m_serviceSelector->itemData(m_serviceSelector->currentIndex()).toMap().value("service").toString();
}

QString SearchDialog::query() const {
    return m_queryEdit->text();
}

void SearchDialog::setQuery(const QString &query) {
    m_queryEdit->setText(query);
}

QString SearchDialog::order() const {
    return m_serviceSelector->itemData(m_serviceSelector->currentIndex()).toMap().value("order").toString();
}

void SearchDialog::onQueryChanged(const QString &query) {
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!query.isEmpty());
}
