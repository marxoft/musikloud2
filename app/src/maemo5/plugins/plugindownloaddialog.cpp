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

#include "plugindownloaddialog.h"
#include "categorynamemodel.h"
#include "settings.h"
#include "valueselector.h"
#include <QScrollArea>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>

PluginDownloadDialog::PluginDownloadDialog(const QString &service, QWidget *parent) :
    Dialog(parent),
    m_streamModel(new PluginStreamModel(this)),
    m_categoryModel(new CategoryNameModel(this)),
    m_scrollArea(new QScrollArea(this)),
    m_commandCheckBox(new QCheckBox(tr("Override global custom command"), this)),
    m_commandEdit(new QLineEdit(this)),
    m_streamSelector(new ValueSelector(tr("Audio format"), this)),
    m_categorySelector(new ValueSelector(tr("Category"), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QHBoxLayout(this))
{
    setWindowTitle(tr("Download"));
    setMinimumHeight(360);
    
    m_streamModel->setService(service);
    
    m_streamSelector->setModel(m_streamModel);
    m_categorySelector->setModel(m_categoryModel);
    m_categorySelector->setValue(Settings::defaultCategory());
    m_categorySelector->setEnabled(m_categoryModel->rowCount() > 0);
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
    vbox->addWidget(m_streamSelector);
    vbox->addWidget(m_categorySelector);
    vbox->addWidget(new QLabel(tr("Custom command (%f for filename)"), this));
    vbox->addWidget(m_commandEdit);
    vbox->addWidget(m_commandCheckBox);
    vbox->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    
    m_layout->addWidget(m_scrollArea);
    m_layout->addWidget(m_buttonBox, Qt::AlignBottom);
    m_layout->setStretch(0, 1);
    
    connect(m_streamModel, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
            SLOT(onStreamModelStatusChanged(ResourcesRequest::Status)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));    
}

QString PluginDownloadDialog::trackId() const {
    return m_trackId;
}

QString PluginDownloadDialog::streamId() const {
    return m_streamSelector->currentValue().toMap().value("id").toString();
}

QString PluginDownloadDialog::category() const {
    return m_categorySelector->valueText();
}

QString PluginDownloadDialog::customCommand() const {
    return m_commandEdit->text();
}

bool PluginDownloadDialog::customCommandOverrideEnabled() const {
    return m_commandCheckBox->isChecked();
}

void PluginDownloadDialog::accept() {
    Settings::setDefaultDownloadFormat(m_streamModel->service(), m_streamSelector->valueText());
    Settings::setDefaultCategory(category());
    Dialog::accept();
}

void PluginDownloadDialog::list(const QString &trackId, bool listStreams) {
    m_trackId = trackId;
    
    if (listStreams) {
        m_streamModel->list(trackId);
    }
    else {
        m_streamModel->clear();
        m_streamModel->append(tr("Default format"), QVariant());
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void PluginDownloadDialog::onStreamModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        showProgressIndicator();
        return;
    case ResourcesRequest::Ready:
        if (m_streamModel->rowCount() > 0) {
            m_streamSelector->setCurrentIndex(qMax(0, m_streamModel->match("name",
                                                   Settings::defaultDownloadFormat(m_streamModel->service()))));
        }
        else {
            QMessageBox::critical(this, tr("Error"), tr("No streams available"));
        }
        
        break;
    case ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), tr("No streams available"));
        break;
    default:
        break;
    }
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_streamModel->rowCount() > 0);
    hideProgressIndicator();
}
