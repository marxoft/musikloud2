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
#include <QInputDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMaemo5InformationBox>
#include <QMessageBox>
#include <QFileDialog>

MainWindow* MainWindow::self = 0;

MainWindow::MainWindow() :
    StackedWindow(),
    m_serviceModel(new ServiceModel(this)),
    m_serviceAction(new ValueSelectorAction(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_playFolderAction(new QAction(tr("Play folder"), this)),
    m_queueFolderAction(new QAction(tr("Queue folder"), this)),
    m_playUrlAction(new QAction(tr("Play URL"), this)),
    m_queueUrlAction(new QAction(tr("Queue URL"), this)),
    m_transfersAction(new QAction(tr("Transfers"), this)),
    m_settingsAction(new QAction(tr("Settings"), this)),
    m_aboutAction(new QAction(tr("About"), this))
{
    m_serviceAction->setText(tr("Service"));
    m_serviceAction->setModel(m_serviceModel);
    m_serviceAction->setValue(Settings::currentService());
    
    menuBar()->addAction(m_serviceAction);
    menuBar()->addAction(m_playFolderAction);
    menuBar()->addAction(m_queueFolderAction);
    menuBar()->addAction(m_playUrlAction);
    menuBar()->addAction(m_queueUrlAction);
    menuBar()->addAction(m_transfersAction);
    menuBar()->addAction(m_settingsAction);
    menuBar()->addAction(m_aboutAction);
    menuBar()->addAction(m_nowPlayingAction);
    
    connect(m_serviceAction, SIGNAL(valueChanged(QVariant)), this, SLOT(setService(QVariant)));
    connect(m_playFolderAction, SIGNAL(triggered()), this, SLOT(playFolder()));
    connect(m_queueFolderAction, SIGNAL(triggered()), this, SLOT(queueFolder()));
    connect(m_playUrlAction, SIGNAL(triggered()), this, SLOT(playUrl()));
    connect(m_queueUrlAction, SIGNAL(triggered()), this, SLOT(queueUrl()));
    connect(m_transfersAction, SIGNAL(triggered()), this, SLOT(showTransfers()));
    connect(m_settingsAction, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    connect(AudioPlayer::instance(), SIGNAL(statusChanged(AudioPlayer::Status)),
            this, SLOT(onPlayerStatusChanged(AudioPlayer::Status)));
    connect(Transfers::instance(), SIGNAL(transferAdded(Transfer*)), this, SLOT(onTransferAdded(Transfer*)));
    
    setService(Settings::currentService());
}

MainWindow::~MainWindow() {
    self = 0;
}

MainWindow* MainWindow::instance() {
    return self ? self : self = new MainWindow;
}

bool MainWindow::search(const QString &service, const QString &query, const QString &type, const QString &order) {
    clearWindows();
    
    if (service != Settings::currentService()) {
        setService(service);
    }
    
    return QMetaObject::invokeMethod(centralWidget(), "search", Qt::QueuedConnection, Q_ARG(QString, query),
                                     Q_ARG(QString, type), Q_ARG(QString, order));
}

bool MainWindow::showResource(const QString &url) {
    const QVariantMap resource = Resources::getResourceFromUrl(url);
    
    if (resource.isEmpty()) {
        return false;
    }
    
    return showResource(resource);
}

bool MainWindow::showResource(const QVariantMap &resource) {
    clearWindows();
    activateWindow();
    
    const QVariant service = resource.value("service");
    
    if (service != Settings::currentService()) {
        setService(service);
    }
    
    return QMetaObject::invokeMethod(centralWidget(), "showResource", Qt::QueuedConnection,
                                     Q_ARG(QString, resource.value("type").toString()),
                                     Q_ARG(QString, resource.value("id").toString()));
}

void MainWindow::playFolder() {
    const QString folder = QFileDialog::getExistingDirectory(this, tr("Play folder"), "/home/user/MyDocs/",
                                                             QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
    
    if (!folder.isEmpty()) {
        if (AudioPlayer::instance()->playUrl(QUrl::fromLocalFile(folder)) > 0) {
            NowPlayingWindow *window = new NowPlayingWindow(this);
            window->show();
        }
        else {
            QMaemo5InformationBox::information(this, tr("No tracks added"));
        }
    }
}

void MainWindow::playUrl() {
    const QString url = QInputDialog::getText(this, tr("Play URL"), tr("URL"));
    
    if (!url.isEmpty()) {
        if (AudioPlayer::instance()->playUrl(url) > 0) {
            NowPlayingWindow *window = new NowPlayingWindow(this);
            window->show();
        }
        else {
            QMaemo5InformationBox::information(this, tr("No tracks added"));
        }
    }
}

void MainWindow::queueFolder() {
    const QString folder = QFileDialog::getExistingDirectory(this, tr("Queue folder"), "/home/user/MyDocs/",
                                                             QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
    
    if (!folder.isEmpty()) {
        const int added = AudioPlayer::instance()->addUrl(QUrl::fromLocalFile(folder));
        
        if (added > 0) {
            QMaemo5InformationBox::information(this, tr("%1 tracks added to playback queue").arg(added));
        }
        else {
            QMaemo5InformationBox::information(this, tr("No tracks added"));
        }
    }
}

void MainWindow::queueUrl() {
    const QString url = QInputDialog::getText(this, tr("Queue URL"), tr("URL"));
    
    if (!url.isEmpty()) {
        const int added = AudioPlayer::instance()->addUrl(url);
        
        if (added > 0) {
            QMaemo5InformationBox::information(this, tr("%1 tracks added to playback queue").arg(added));
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
    Settings::setCurrentService(service.toString());
    
    const QString text = m_serviceModel->data(m_serviceModel->index(m_serviceModel->match("value", service)),
                                              ServiceModel::NameRole).toString();
    
    setWindowTitle(text.isEmpty() ? "MusiKloud" : text);
}

void MainWindow::showAboutDialog() {
    AboutDialog(this).exec();
}

void MainWindow::showSettingsDialog() {
    SettingsDialog(this).exec();
}

void MainWindow::showTransfers() {
    TransfersWindow *window = new TransfersWindow(this);
    window->show();
}

void MainWindow::onPlayerStatusChanged(AudioPlayer::Status status) {
    if (status == AudioPlayer::Failed) {
        QMessageBox::critical(this, tr("Playback error"), AudioPlayer::instance()->errorString());
    }
}

void MainWindow::onTransferAdded(Transfer *transfer) {
    QMaemo5InformationBox::information(this, tr("'%1' added to transfers").arg(transfer->title()));
}
