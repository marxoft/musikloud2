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

#include "soundclouddownloaddialog.h"
#include "categorynamemodel.h"
#include "resources.h"
#include "settings.h"
#include "transfers.h"
#include "valueselector.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>

SoundCloudDownloadDialog::SoundCloudDownloadDialog(const QString &resourceId, const QString &title, QWidget *parent) :
    Dialog(parent),
    m_id(resourceId),
    m_title(title),
    m_streamModel(new SoundCloudStreamModel(this)),
    m_categoryModel(new CategoryNameModel(this)),
    m_streamSelector(new ValueSelector(tr("Audio format"), this)),
    m_categorySelector(new ValueSelector(tr("Category"), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QGridLayout(this))
{
    setWindowTitle(tr("Download track"));
    
    m_streamSelector->setModel(m_streamModel);
    m_categorySelector->setModel(m_categoryModel);
    m_categorySelector->setValue(Settings::instance()->defaultCategory());
    m_categorySelector->setEnabled(m_categoryModel->rowCount() > 0);
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    m_layout->addWidget(m_streamSelector, 0, 0);
    m_layout->addWidget(m_categorySelector, 1, 0);
    m_layout->addWidget(m_buttonBox, 1, 1);
    m_layout->setColumnStretch(0, 1);
    
    connect(m_streamModel, SIGNAL(statusChanged(QSoundCloud::StreamsRequest::Status)), this,
            SLOT(onStreamModelStatusChanged(QSoundCloud::StreamsRequest::Status)));
    connect(m_categorySelector, SIGNAL(valueChanged(QVariant)), this, SLOT(onCategoryChanged()));
    connect(m_streamSelector, SIGNAL(valueChanged(QVariant)), this, SLOT(onStreamChanged()));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(addDownload()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));    
}

void SoundCloudDownloadDialog::showEvent(QShowEvent *e) {
    Dialog::showEvent(e);
    m_streamModel->get(m_id);
}

void SoundCloudDownloadDialog::onCategoryChanged() {
    Settings::instance()->setDefaultCategory(m_categorySelector->valueText());
}

void SoundCloudDownloadDialog::onStreamChanged() {
    Settings::instance()->setDefaultDownloadFormat(Resources::SOUNDCLOUD, m_streamSelector->valueText());
}

void SoundCloudDownloadDialog::onStreamModelStatusChanged(QSoundCloud::StreamsRequest::Status status) {
    switch (status) {
    case QSoundCloud::StreamsRequest::Loading:
        showProgressIndicator();
        return;
    case QSoundCloud::StreamsRequest::Ready:
        if (m_streamModel->rowCount() > 0) {
            m_streamSelector->setCurrentIndex(qMax(0, m_streamModel->match("name",
                                                   Settings::instance()->defaultDownloadFormat(Resources::SOUNDCLOUD))));
        }
        else {
            QMessageBox::critical(this, tr("Error"), tr("No streams available for '%1'").arg(m_title));
        }
        
        break;
    case QSoundCloud::StreamsRequest::Failed:
        QMessageBox::critical(this, tr("Error"), tr("No streams available for '%1'").arg(m_title));
        break;
    default:
        break;
    }
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_streamModel->rowCount() > 0);
    hideProgressIndicator();
}

void SoundCloudDownloadDialog::addDownload() {
    QString streamId = m_streamSelector->currentValue().toMap().value("id").toString();
    QString category = m_categorySelector->valueText();
    Transfers::instance()->addDownloadTransfer(Resources::SOUNDCLOUD, m_id, streamId, QUrl(), m_title, category);
    accept();
}
