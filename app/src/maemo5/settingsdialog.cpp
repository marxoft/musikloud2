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

#include "settingsdialog.h"
#include "categoriesdialog.h"
#include "concurrenttransfersmodel.h"
#include "listview.h"
#include "networkproxydialog.h"
#include "pluginconfigmodel.h"
#include "pluginsettingsdialog.h"
#include "settings.h"
#include "valueselector.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMaemo5InformationBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    Dialog(parent),
    m_transfersModel(new ConcurrentTransfersModel(this)),
    m_pluginModel(new PluginConfigModel(this)),
    m_transfersSelector(new ValueSelector(tr("Maximum concurrent transfers"), this)),
    m_pluginView(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_downloadPathSelector(new QMaemo5ValueButton(tr("Default download path"), this)),
    m_restoreQueueCheckBox(new QCheckBox(tr("Restore playback queue on startup"), this)),
    m_clipboardCheckBox(new QCheckBox(tr("Monitor clipboard for URLs"), this)),
    m_transfersCheckBox(new QCheckBox(tr("Start transfers automatically"), this)),
    m_sleepTimerSpinBox(new QSpinBox(this)),
    m_categoriesButton(new QPushButton(tr("Categories"), this)),
    m_proxyButton(new QPushButton(tr("Network proxy"), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QHBoxLayout(this))
{
    setWindowTitle(tr("Settings"));
    setMinimumHeight(360);
    
    m_downloadPathSelector->setValueText(Settings::downloadPath());
    m_transfersSelector->setModel(m_transfersModel);
    m_transfersSelector->setValue(Settings::maximumConcurrentTransfers());
    m_pluginView->setModel(m_pluginModel);
    m_pluginView->setFixedHeight(m_pluginModel->rowCount() > 0
                                 ? m_pluginModel->rowCount() * m_pluginView->sizeHintForRow(0) : 0);
    m_restoreQueueCheckBox->setChecked(Settings::restorePlaybackQueueOnStartup());
    m_clipboardCheckBox->setChecked(Settings::clipboardMonitorEnabled());
    m_transfersCheckBox->setChecked(Settings::startTransfersAutomatically());
    m_sleepTimerSpinBox->setRange(1, 1000);
    m_sleepTimerSpinBox->setSuffix(tr(" minutes"));
    m_sleepTimerSpinBox->setValue(Settings::sleepTimerDuration());
    
    const QString midColor = palette().color(QPalette::Mid).name();
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
    vbox->addWidget(new QLabel(QString("<p style='color: %1'>%2</p>").arg(midColor).arg(tr("Media/content")), this),
                    0, Qt::AlignHCenter);
    vbox->addWidget(m_downloadPathSelector);
    vbox->addWidget(m_clipboardCheckBox);
    vbox->addWidget(m_restoreQueueCheckBox);
    vbox->addWidget(new QLabel(tr("Sleep timer duration"), this));
    vbox->addWidget(m_sleepTimerSpinBox);
    vbox->addWidget(new QLabel(QString("<p style='color: %1'>%2</p>").arg(midColor).arg(tr("Transfers")), this),
                    0, Qt::AlignHCenter);
    vbox->addWidget(m_transfersCheckBox);
    vbox->addWidget(m_transfersSelector);
    vbox->addWidget(m_proxyButton);
    vbox->addWidget(m_categoriesButton);
    vbox->addWidget(new QLabel(QString("<p style='color: %1'>%2</p>").arg(midColor).arg(tr("Plugins")), this),
                    0, Qt::AlignHCenter);
    vbox->addWidget(m_pluginView);
    vbox->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    
    m_layout->addWidget(m_scrollArea);
    m_layout->addWidget(m_buttonBox);
    
    connect(m_downloadPathSelector, SIGNAL(clicked()), this, SLOT(showFileDialog()));
    connect(m_restoreQueueCheckBox, SIGNAL(toggled(bool)),
            Settings::instance(), SLOT(setRestorePlaybackQueueOnStartup(bool)));
    connect(m_clipboardCheckBox, SIGNAL(toggled(bool)),
            Settings::instance(), SLOT(setClipboardMonitorEnabled(bool)));
    connect(m_transfersCheckBox, SIGNAL(toggled(bool)),
            Settings::instance(), SLOT(setStartTransfersAutomatically(bool)));
    connect(m_sleepTimerSpinBox, SIGNAL(valueChanged(int)),
            Settings::instance(), SLOT(setSleepTimerDuration(int)));
    connect(m_proxyButton, SIGNAL(clicked()), this, SLOT(showNetworkProxyDialog()));
    connect(m_categoriesButton, SIGNAL(clicked()), this, SLOT(showCategoriesDialog()));
    connect(m_pluginView, SIGNAL(activated(QModelIndex)), this, SLOT(showPluginDialog(QModelIndex)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void SettingsDialog::showCategoriesDialog() {
    CategoriesDialog(this).exec();
}

void SettingsDialog::showFileDialog() {
    const QString path = QFileDialog::getExistingDirectory(this, tr("Default download path"),
                                                           Settings::downloadPath());

    if (!path.isEmpty()) {
        m_downloadPathSelector->setValueText(path);
        Settings::setDownloadPath(path);
    }
}

void SettingsDialog::showNetworkProxyDialog() {
    NetworkProxyDialog(this).exec();
}

void SettingsDialog::showPluginDialog(const QModelIndex &index) {
    const QVariantList settings = index.data(PluginConfigModel::SettingsRole).toList();

    if (settings.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("No settings for this plugin"));
    }
    else {
        PluginSettingsDialog dialog(index.data(PluginConfigModel::IdRole).toString(), settings, this);
        dialog.setWindowTitle(index.data(PluginConfigModel::DisplayNameRole).toString());
        dialog.exec();
    }
}
