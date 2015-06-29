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

#include "mainwindow.h"
#include "aboutdialog.h"
#include "nowplayingaction.h"
#include "nowplayingwindow.h"
#include "pluginview.h"
#include "resources.h"
#include "servicemodel.h"
#include "settings.h"
#include "settingsdialog.h"
#include "soundcloudview.h"
#include "transfers.h"
#include "transferswindow.h"
#include "valueselectoraction.h"
#include <QLabel>
#include <QMenuBar>
#include <QMaemo5InformationBox>
#include <QMessageBox>
#include <QFileDialog>

MainWindow* MainWindow::self = 0;

MainWindow::MainWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_serviceModel(new ServiceModel(this)),
    m_serviceAction(new ValueSelectorAction(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_queueAction(new QAction(tr("Queue folder"), this)),
    m_playAction(new QAction(tr("Play folder"), this)),
    m_transfersAction(new QAction(tr("Transfers"), this)),
    m_settingsAction(new QAction(tr("Settings"), this)),
    m_aboutAction(new QAction(tr("About"), this))
{
    if (!self) {
        self = this;
    }
        
    m_serviceAction->setText(tr("Service"));
    m_serviceAction->setModel(m_serviceModel);
    m_serviceAction->setValue(Settings::instance()->currentService());
    
    menuBar()->addAction(m_serviceAction);
    menuBar()->addAction(m_queueAction);
    menuBar()->addAction(m_playAction);
    menuBar()->addAction(m_transfersAction);
    menuBar()->addAction(m_settingsAction);
    menuBar()->addAction(m_aboutAction);
    menuBar()->addAction(m_nowPlayingAction);
    
    connect(m_serviceAction, SIGNAL(valueChanged(QVariant)), this, SLOT(setService(QVariant)));
    connect(m_queueAction, SIGNAL(triggered()), this, SLOT(queueFolder()));
    connect(m_playAction, SIGNAL(triggered()), this, SLOT(playFolder()));
    connect(m_transfersAction, SIGNAL(triggered()), this, SLOT(showTransfers()));
    connect(m_settingsAction, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    connect(AudioPlayer::instance(), SIGNAL(statusChanged(AudioPlayer::Status)),
            this, SLOT(onPlayerStatusChanged(AudioPlayer::Status)));
    connect(Transfers::instance(), SIGNAL(transferAdded(Transfer*)), this, SLOT(onTransferAdded(Transfer*)));
    
    setService(Settings::instance()->currentService());
}

MainWindow::~MainWindow() {
    if (self == this) {
        self = 0;
    }
}

MainWindow* MainWindow::instance() {
    return self;
}

bool MainWindow::search(const QString &service, const QString &query, const QString &type, const QString &order) {
    clearWindows();
    
    if (service != Settings::instance()->currentService()) {
        setService(service);
    }
    
    return QMetaObject::invokeMethod(centralWidget(), "search", Qt::QueuedConnection, Q_ARG(QString, query),
                                     Q_ARG(QString, type), Q_ARG(QString, order));
}

bool MainWindow::showResource(const QString &url) {
    QVariantMap resource = Resources::getResourceFromUrl(url);
    
    if (resource.isEmpty()) {
        return false;
    }
    
    return showResource(resource);
}

bool MainWindow::showResource(const QVariantMap &resource) {
    clearWindows();
    activateWindow();
    
    QVariant service = resource.value("service");
    
    if (service != Settings::instance()->currentService()) {
        setService(service);
    }
    
    return QMetaObject::invokeMethod(centralWidget(), "showResource", Qt::QueuedConnection,
                                     Q_ARG(QString, resource.value("type").toString()),
                                     Q_ARG(QString, resource.value("id").toString()));
}

void MainWindow::playFolder() {
    QString folder = QFileDialog::getExistingDirectory(this, tr("Play folder"), "/home/user/MyDocs/",
                                                       QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
    
    if (!folder.isEmpty()) {
        if (AudioPlayer::instance()->playFolder(folder)) {
            NowPlayingWindow *window = new NowPlayingWindow(this);
            window->show();
        }
        else {
            QMaemo5InformationBox::information(this, tr("No tracks added"));
        }
    }
}

void MainWindow::queueFolder() {
    QString folder = QFileDialog::getExistingDirectory(this, tr("Queue folder"), "/home/user/MyDocs/",
                                                       QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
    
    if (!folder.isEmpty()) {
        const int count = AudioPlayer::instance()->queueCount();
        
        if (AudioPlayer::instance()->addFolder(folder)) {
            QMaemo5InformationBox::information(this, tr("%1 tracks added to playback queue")
                                              .arg(AudioPlayer::instance()->queueCount() - count));
        }
        else {
            QMaemo5InformationBox::information(this, tr("No tracks added"));
        }
    }
}

void MainWindow::setService(const QVariant &service) {
    if (service == Resources::SOUNDCLOUD) {
        setCentralWidget(new SoundCloudView(this));
    }
    else {
        setCentralWidget(new PluginView(service.toString(), this));
    }
    
    m_serviceAction->setValue(service);
    Settings::instance()->setCurrentService(service.toString());
    
    QString text = m_serviceModel->data(m_serviceModel->index(m_serviceModel->match("value", service)),
                                        ServiceModel::NameRole).toString();
    
    setWindowTitle(text.isEmpty() ? "MusiKloud" : text);
}

void MainWindow::showAboutDialog() {
    AboutDialog *dialog = new AboutDialog(this);
    dialog->open();
}

void MainWindow::showSettingsDialog() {
    SettingsDialog *dialog = new SettingsDialog(this);
    dialog->open();
}

void MainWindow::showTransfers() {
    TransfersWindow *window = new TransfersWindow(this);
    window->show();
}

void MainWindow::onPlayerStatusChanged(AudioPlayer::Status status) {
    if (status == AudioPlayer::Failed) {
        QMessageBox::critical(this, tr("Error"), AudioPlayer::instance()->errorString());
    }
}

void MainWindow::onTransferAdded(Transfer *transfer) {
    QMaemo5InformationBox::information(this, tr("'%1' added to transfers").arg(transfer->title()));
}
