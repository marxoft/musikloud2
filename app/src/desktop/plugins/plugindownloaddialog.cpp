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
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

PluginDownloadDialog::PluginDownloadDialog(const QString &service, QWidget *parent) :
    QDialog(parent),
    m_streamModel(new PluginStreamModel(this)),
    m_categoryModel(new CategoryNameModel(this)),
    m_streamSelector(new QComboBox(this)),
    m_categorySelector(new QComboBox(this)),
    m_commandCheckBox(new QCheckBox(tr("&Override global custom command"), this)),
    m_commandEdit(new QLineEdit(this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)),
    m_layout(new QFormLayout(this))
{
    setWindowTitle(tr("Download track"));
    
    m_streamModel->setService(service);

    m_streamSelector->setModel(m_streamModel);

    m_categorySelector->setModel(m_categoryModel);

    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    m_layout->addRow(tr("Audio &format:"), m_streamSelector);
    m_layout->addRow(tr("&Category:"), m_categorySelector);
    m_layout->addRow(tr("&Custom command:"), m_commandEdit);
    m_layout->addRow(m_commandCheckBox);
    m_layout->addRow(m_buttonBox);

    connect(m_streamModel, SIGNAL(statusChanged(ResourcesRequest::Status)),
            this, SLOT(onStreamModelStatusChanged(ResourcesRequest::Status)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString PluginDownloadDialog::trackId() const {
    return m_trackId;
}

QString PluginDownloadDialog::streamId() const {
    return m_streamSelector->itemData(m_streamSelector->currentIndex()).toMap().value("id").toString();
}

QString PluginDownloadDialog::category() const {
    return m_categorySelector->currentText();
}

QString PluginDownloadDialog::customCommand() const {
    return m_commandEdit->text();
}

bool PluginDownloadDialog::customCommandOverrideEnabled() const {
    return m_commandCheckBox->isChecked();
}

void PluginDownloadDialog::accept() {
    Settings::setDefaultDownloadFormat(m_streamModel->service(), m_streamSelector->currentText());
    Settings::setDefaultCategory(category());
    QDialog::accept();
}

void PluginDownloadDialog::list(const QString &trackId, bool listStreams) {
    m_trackId = trackId;
    
    if (listStreams) {
        m_streamModel->list(trackId);
    }
    else {
        m_streamModel->clear();
        m_streamModel->append(tr("Default format"), QVariant());
    }
}

void PluginDownloadDialog::onStreamModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        break;
    case ResourcesRequest::Ready:
        m_streamSelector->setCurrentIndex(qMax(0, m_streamModel->match("name",
                                          Settings::defaultDownloadFormat(m_streamModel->service()))));
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        break;
    case ResourcesRequest::Failed:
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        QMessageBox::critical(this, tr("Error"), m_streamModel->errorString());
        break;
    default:
        break;
    }
}
