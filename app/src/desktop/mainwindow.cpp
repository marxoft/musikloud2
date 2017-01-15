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

#include "mainwindow.h"
#include "aboutdialog.h"
#include "definitions.h"
#include "itemmetadata.h"
#include "playbackqueuepage.h"
#include "pluginmanager.h"
#include "plugintrackspage.h"
#include "searchdialog.h"
#include "settings.h"
#include "settingsdialog.h"
#include "soundcloudtrackspage.h"
#include "transfers.h"
#include "transferspage.h"
#include "utils.h"
#include <QFileDialog>
#include <QGridLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSlider>
#include <QStatusBar>
#include <QTabBar>
#include <QTabWidget>
#include <QToolBar>
#include <QToolButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_fileMenu(new QMenu(tr("&File"), this)),
    m_viewMenu(new QMenu(tr("&View"), this)),
    m_playbackMenu(new QMenu(tr("&Playback"), this)),
    m_transfersMenu(new QMenu(tr("&Downloads"), this)),
    m_editMenu(new QMenu(tr("&Edit"), this)),
    m_helpMenu(new QMenu(tr("&About"), this)),
    m_openFilesAction(new QAction(QIcon::fromTheme("document-open"), tr("&Open files"), this)),
    m_openUrlAction(new QAction(QIcon::fromTheme("folder-remote"), tr("Open &URL"), this)),
    m_addFilesAction(new QAction(QIcon::fromTheme("list-add"), tr("&Add files"), this)),
    m_addUrlAction(new QAction(QIcon::fromTheme("folder-remote"), tr("Add &URL"), this)),
    m_searchAction(new QAction(QIcon::fromTheme("edit-find"), tr("&Search"), this)),
    m_pluginsAction(new QAction(QIcon::fromTheme("view-refresh"), tr("Load &plugins"), this)),
    m_quitAction(new QAction(QIcon::fromTheme("application-exit"), tr("&Quit"), this)),
    m_showPlaybackQueueAction(new QAction(QIcon::fromTheme("music-library"), tr("Show &playback queue"), this)),
    m_showTransfersAction(new QAction(QIcon::fromTheme("folder-publicshare"), tr("Show &transfers"), this)),
    m_reloadAction(new QAction(QIcon::fromTheme("view-refresh"), tr("&Reload current tab"), this)),
    m_closeAction(new QAction(QIcon::fromTheme("list-remove"), tr("&Close current tab"), this)),
    m_playPauseAction(new QAction(QIcon::fromTheme("media-playback-start"), tr("&Play"), this)),
    m_stopAction(new QAction(QIcon::fromTheme("media-playback-stop"), tr("&Stop"), this)),
    m_previousAction(new QAction(QIcon::fromTheme("media-skip-backward"), tr("&Previous"), this)),
    m_nextAction(new QAction(QIcon::fromTheme("media-skip-forward"), tr("&Next"), this)),
    m_repeatAction(new QAction(QIcon::fromTheme("media-playlist-repeat"), tr("&Repeat"), this)),
    m_shuffleAction(new QAction(QIcon::fromTheme("media-playlist-shuffle"), tr("&Shuffle"), this)),
    m_stopAfterCurrentAction(new QAction(tr("Stop &after current track"), this)),
    m_clearAction(new QAction(QIcon::fromTheme("list-remove"), tr("&Clear playback queue"), this)),
    m_startTransfersAction(new QAction(QIcon::fromTheme("media-playback-start"), tr("&Start transfers"), this)),
    m_pauseTransfersAction(new QAction(QIcon::fromTheme("media-playback-pause"), tr("&Pause transfers"), this)),
    m_settingsAction(new QAction(QIcon::fromTheme("preferences-desktop"), tr("&Preferences"), this)),
    m_aboutAction(new QAction(QIcon::fromTheme("help-about"), tr("&About"), this)),
    m_positionSpacer(new QWidget(this)),
    m_positionSpacerAction(0),
    m_positionSlider(new QSlider(Qt::Horizontal, this)),
    m_positionSliderAction(0),
    m_positionLabel(new QLabel(this)),
    m_positionLabelAction(0),
    m_toolBar(new QToolBar(this)),
    m_widget(new QWidget(this)),
    m_tabWidget(new QTabWidget(m_widget)),
    m_nowPlayingMetaData(new ItemMetaData(m_widget)),
    m_layout(new QVBoxLayout(m_widget))
{
    setWindowTitle("MusiKloud2");
    
    // Initialise status bar
    statusBar();
    
    // Add menus
    menuBar()->addMenu(m_fileMenu);
    menuBar()->addMenu(m_viewMenu);
    menuBar()->addMenu(m_playbackMenu);
    menuBar()->addMenu(m_transfersMenu);
    menuBar()->addMenu(m_editMenu);
    menuBar()->addMenu(m_helpMenu);
    
    // Setup file menu
    m_fileMenu->addAction(m_openFilesAction);
    m_fileMenu->addAction(m_openUrlAction);
    m_fileMenu->addAction(m_addFilesAction);
    m_fileMenu->addAction(m_addUrlAction);
    m_fileMenu->addAction(m_searchAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_pluginsAction);
    m_fileMenu->addAction(m_quitAction);
    
    // Setup file menu actions
    m_openFilesAction->setShortcut(tr("Ctrl+O"));
    m_openUrlAction->setShortcut(tr("Ctrl+U"));
    m_addFilesAction->setShortcut(tr("Ctrl+Shift+O"));
    m_addUrlAction->setShortcut(tr("Ctrl+Shift+U"));
    m_searchAction->setShortcut(tr("Ctrl+Y"));
    m_pluginsAction->setShortcut(tr("Ctrl+L"));
    m_quitAction->setShortcut(tr("Ctrl+Q"));
    
    // Setup view menu
    m_viewMenu->addAction(m_showPlaybackQueueAction);
    m_viewMenu->addAction(m_showTransfersAction);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_reloadAction);
    m_viewMenu->addAction(m_closeAction);
    
    // Setup view menu actions
    m_reloadAction->setEnabled(false);
    m_closeAction->setShortcut(tr("Ctrl+W"));
    m_closeAction->setEnabled(false);
    
    // Setup playback menu
    m_playbackMenu->addAction(m_playPauseAction);
    m_playbackMenu->addAction(m_stopAction);
    m_playbackMenu->addAction(m_previousAction);
    m_playbackMenu->addAction(m_nextAction);
    m_playbackMenu->addSeparator();
    m_playbackMenu->addAction(m_repeatAction);
    m_playbackMenu->addAction(m_shuffleAction);
    m_playbackMenu->addAction(m_stopAfterCurrentAction);
    m_playbackMenu->addSeparator();
    m_playbackMenu->addAction(m_clearAction);
    
    // Setup playback menu actions
    m_playPauseAction->setShortcut(tr("Ctrl+Return"));
    m_stopAction->setShortcut(tr("Ctrl+."));
    m_previousAction->setShortcut(tr("Alt+Up"));
    m_nextAction->setShortcut(tr("Alt+Down"));
    m_repeatAction->setCheckable(true);
    m_repeatAction->setShortcut(tr("Ctrl+R"));
    m_repeatAction->setToolTip(tr("Toggle repeat playback mode"));
    m_shuffleAction->setCheckable(true);
    m_shuffleAction->setShortcut(tr("Ctrl+S"));
    m_shuffleAction->setToolTip(tr("Toggle shuffle playback mode"));
    m_stopAfterCurrentAction->setCheckable(true);
    m_clearAction->setShortcut(tr("Ctrl+Shift+X"));
    
    // Setup transfers menu
    m_transfersMenu->addAction(m_startTransfersAction);
    m_transfersMenu->addAction(m_pauseTransfersAction);
    
    // Setup edit menu
    m_editMenu->addAction(m_settingsAction);
    
    // Setup edit menu actions
    m_settingsAction->setShortcut(tr("Ctrl+P"));
    
    // Setup help menu
    m_helpMenu->addAction(m_aboutAction);

    // Setup toolbar
    m_toolBar->setObjectName("mainToolBar");
    m_toolBar->setWindowTitle(tr("Main toolbar"));
    m_toolBar->setMovable(false);
    m_toolBar->setAllowedAreas(Qt::TopToolBarArea);
    m_toolBar->addAction(m_openFilesAction);
    m_toolBar->addAction(m_addFilesAction);
    m_toolBar->addAction(m_searchAction);
    m_toolBar->addAction(m_previousAction);
    m_toolBar->addAction(m_nextAction);
    m_toolBar->addAction(m_playPauseAction);
    m_toolBar->addAction(m_stopAction);
    m_positionSpacerAction = m_toolBar->addWidget(m_positionSpacer);
    m_positionSliderAction = m_toolBar->addWidget(m_positionSlider);
    m_positionLabelAction = m_toolBar->addWidget(m_positionLabel);
    m_toolBar->addAction(m_repeatAction);
    m_toolBar->addAction(m_shuffleAction);
    addToolBar(Qt::TopToolBarArea, m_toolBar);
    
    // Setup position spacer, slider and label
    m_positionSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_positionSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_positionSliderAction->setVisible(false);
    m_positionLabel->setFixedWidth(m_positionLabel->fontMetrics().width("000:00 / 000:00"));
    m_positionLabel->setAlignment(Qt::AlignCenter);
    m_positionLabelAction->setVisible(false);
    
    // Setup tab widget
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->tabBar()->setSelectionBehaviorOnRemove(QTabBar::SelectLeftTab);
    m_tabWidget->tabBar()->setExpanding(false);
    
    // Setup now playing metadata
    m_nowPlayingMetaData->setStyleSheet("font-weight: bold");
    m_nowPlayingMetaData->setThumbnailSize(QSize(96, 96));
    m_nowPlayingMetaData->hide();
    
    // Setup layout
    m_layout->addWidget(m_tabWidget);
    m_layout->addWidget(m_nowPlayingMetaData);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(m_widget);
    
    // Restore window geometry/state
    restoreGeometry(Settings::mainWindowGeometry());
    restoreState(Settings::mainWindowState());
    
    // Connect signals and slots
    connect(AudioPlayer::instance(), SIGNAL(durationChanged(qint64)), this, SLOT(onPlayerDurationChanged(qint64)));
    connect(AudioPlayer::instance(), SIGNAL(metaDataChanged()), this, SLOT(updateNowPlayingMetaData()));
    connect(AudioPlayer::instance(), SIGNAL(positionChanged(qint64)), this, SLOT(onPlayerPositionChanged(qint64)));
    connect(AudioPlayer::instance(), SIGNAL(seekableChanged(bool)), this, SLOT(onPlayerSeekableChanged(bool)));
    connect(AudioPlayer::instance(), SIGNAL(statusChanged(AudioPlayer::Status)),
            this, SLOT(onPlayerStatusChanged(AudioPlayer::Status)));
    connect(m_openFilesAction, SIGNAL(triggered()), this, SLOT(openFiles()));
    connect(m_openUrlAction, SIGNAL(triggered()), this, SLOT(openUrl()));
    connect(m_addFilesAction, SIGNAL(triggered()), this, SLOT(addFiles()));
    connect(m_addUrlAction, SIGNAL(triggered()), this, SLOT(addUrl()));
    connect(m_searchAction, SIGNAL(triggered()), this, SLOT(search()));
    connect(m_pluginsAction, SIGNAL(triggered()), this, SLOT(loadPlugins()));
    connect(m_quitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_showPlaybackQueueAction, SIGNAL(triggered()), this, SLOT(showPlaybackQueue()));
    connect(m_showTransfersAction, SIGNAL(triggered()), this, SLOT(showTransfers()));
    connect(m_reloadAction, SIGNAL(triggered()), this, SLOT(reloadCurrentTab()));
    connect(m_closeAction, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));
    connect(m_playPauseAction, SIGNAL(triggered()), AudioPlayer::instance(), SLOT(togglePlaying()));
    connect(m_stopAction, SIGNAL(triggered()), AudioPlayer::instance(), SLOT(stop()));
    connect(m_previousAction, SIGNAL(triggered()), AudioPlayer::instance(), SLOT(previous()));
    connect(m_nextAction, SIGNAL(triggered()), AudioPlayer::instance(), SLOT(next()));
    connect(m_repeatAction, SIGNAL(triggered(bool)), AudioPlayer::instance(), SLOT(setRepeatEnabled(bool)));
    connect(m_shuffleAction, SIGNAL(triggered(bool)), AudioPlayer::instance(), SLOT(setShuffleEnabled(bool)));
    connect(m_stopAfterCurrentAction, SIGNAL(triggered(bool)),
            AudioPlayer::instance(), SLOT(setStopAfterCurrentTrack(bool)));
    connect(m_clearAction, SIGNAL(triggered()), AudioPlayer::instance(), SLOT(clearQueue()));
    connect(m_startTransfersAction, SIGNAL(triggered()), Transfers::instance(), SLOT(start()));
    connect(m_pauseTransfersAction, SIGNAL(triggered()), Transfers::instance(), SLOT(pause()));
    connect(m_settingsAction, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    connect(m_positionSlider, SIGNAL(sliderReleased()), this, SLOT(updatePlayerPosition()));
    connect(m_positionSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabStatusChanged()));
    connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Settings::setMainWindowGeometry(saveGeometry());
    Settings::setMainWindowState(saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::addFiles() {
    QFileDialog dialog(this, tr("Add files"));
    dialog.setOptions(QFileDialog::ReadOnly);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    
    if (dialog.exec() == QDialog::Accepted) {
        AudioPlayer::instance()->addUrls(dialog.selectedUrls());
    }
}

void MainWindow::addUrl() {
    const QString url = QInputDialog::getText(this, tr("Add URL"), tr("URL"));
    
    if (!url.isEmpty()) {
        AudioPlayer::instance()->addUrl(url);
    }
}

void MainWindow::openFiles() {
    QFileDialog dialog(this, tr("Open files"));
    dialog.setOptions(QFileDialog::ReadOnly);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    
    if (dialog.exec() == QDialog::Accepted) {
        AudioPlayer::instance()->playUrls(dialog.selectedUrls());
    }
}

void MainWindow::openUrl() {
    const QString url = QInputDialog::getText(this, tr("Open URL"), tr("URL"));
    
    if (!url.isEmpty()) {
        AudioPlayer::instance()->playUrl(url);
    }
}

void MainWindow::search() {
    SearchDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.service() == Resources::SOUNDCLOUD) {
            SoundCloudTracksPage *page = new SoundCloudTracksPage(m_tabWidget);
            m_tabWidget->addTab(page, tr("Search '%1'").arg(dialog.query()));
            m_tabWidget->setCurrentWidget(page);
            connect(page, SIGNAL(statusChanged(Page::Status)), this, SLOT(onTabStatusChanged()));
            connect(page, SIGNAL(statusTextChanged(QString)), statusBar(), SLOT(showMessage(QString)));
            connect(page, SIGNAL(windowTitleChanged(QString)), this, SLOT(updateTabText(QString)));
            
            QVariantMap filters;
            filters["q"] = dialog.query();
            filters["limit"] = MAX_RESULTS;
            page->get("/tracks", filters);
        }
        else {
            PluginTracksPage *page = new PluginTracksPage(dialog.service(), m_tabWidget);
            m_tabWidget->addTab(page, tr("Search '%1'").arg(dialog.query()));
            m_tabWidget->setCurrentWidget(page);
            connect(page, SIGNAL(statusChanged(Page::Status)), this, SLOT(onTabStatusChanged()));
            connect(page, SIGNAL(statusTextChanged(QString)), statusBar(), SLOT(showMessage(QString)));
            connect(page, SIGNAL(windowTitleChanged(QString)), this, SLOT(updateTabText(QString)));
            page->search(dialog.query(), dialog.order());
        }
        
        m_reloadAction->setEnabled(true);
        m_closeAction->setEnabled(true);
    }
}

void MainWindow::reloadTab(int index) {
    if (Page *page = qobject_cast<Page*>(m_tabWidget->widget(index))) {
        page->reload();
    }
}

void MainWindow::reloadCurrentTab() {
    reloadTab(m_tabWidget->currentIndex());
}

void MainWindow::closeTab(int index) {
    if (QWidget *page = m_tabWidget->widget(index)) {
        m_tabWidget->removeTab(index);
        page->close();
        page->deleteLater();
        
        if (m_tabWidget->count() == 0) {
            m_reloadAction->setEnabled(false);
            m_closeAction->setEnabled(false);
        }
    }
}

void MainWindow::closeCurrentTab() {
    closeTab(m_tabWidget->currentIndex());
}

void MainWindow::updateTabText(const QString &text) {
    if (QWidget *widget = qobject_cast<QWidget*>(sender())) {
        m_tabWidget->setTabText(m_tabWidget->indexOf(widget), text);
    }
}

void MainWindow::onTabStatusChanged() {
    if (Page *page = qobject_cast<Page*>(m_tabWidget->currentWidget())) {
        m_reloadAction->setEnabled(page->status() != Page::Loading);
    }
}

void MainWindow::loadPlugins() {
    const int count = PluginManager::instance()->load();
    
    if (count > 0) {
        QMessageBox::information(this, tr("Load plugins"), tr("%1 new plugin(s) found"));
    }
    else {
        QMessageBox::information(this, tr("Load plugins"), tr("No new plugins found"));
    }
}

void MainWindow::showAboutDialog() {
    AboutDialog(this).exec();
}

void MainWindow::showPlaybackQueue() {
    for (int i = 0; i < m_tabWidget->count(); i++) {
        if (qobject_cast<PlaybackQueuePage*>(m_tabWidget->widget(i))) {
            m_tabWidget->setCurrentIndex(i);
            m_reloadAction->setEnabled(true);
            m_closeAction->setEnabled(true);
            return;
        }
    }
    
    PlaybackQueuePage *page = new PlaybackQueuePage(m_tabWidget);
    m_tabWidget->addTab(page, page->windowTitle());
    m_tabWidget->setCurrentIndex(m_tabWidget->count() - 1);
    connect(page, SIGNAL(statusChanged(Page::Status)), this, SLOT(onTabStatusChanged()));
    connect(page, SIGNAL(statusTextChanged(QString)), statusBar(), SLOT(showMessage(QString)));
    connect(page, SIGNAL(windowTitleChanged(QString)), this, SLOT(updateTabText(QString)));
    m_reloadAction->setEnabled(true);
    m_closeAction->setEnabled(true);
}

void MainWindow::showSettingsDialog() {
    SettingsDialog(this).exec();
}

void MainWindow::showTransfers() {
    for (int i = 0; i < m_tabWidget->count(); i++) {
        if (qobject_cast<TransfersPage*>(m_tabWidget->widget(i))) {
            m_tabWidget->setCurrentIndex(i);
            m_reloadAction->setEnabled(true);
            m_closeAction->setEnabled(true);
            return;
        }
    }
    
    TransfersPage *page = new TransfersPage(m_tabWidget);
    m_tabWidget->addTab(page, page->windowTitle());
    m_tabWidget->setCurrentIndex(m_tabWidget->count() - 1);
    connect(page, SIGNAL(statusChanged(Page::Status)), this, SLOT(onTabStatusChanged()));
    connect(page, SIGNAL(statusTextChanged(QString)), statusBar(), SLOT(showMessage(QString)));
    connect(page, SIGNAL(windowTitleChanged(QString)), this, SLOT(updateTabText(QString)));
    m_reloadAction->setEnabled(true);
    m_closeAction->setEnabled(true);
}

void MainWindow::updateNowPlayingMetaData() {
    if (AudioPlayerMetaData::isAvailable()) {
        QList<ItemMetaDataValue> metaData;
        metaData << ItemMetaDataValue(tr("Title"), AudioPlayerMetaData::title());
        metaData << ItemMetaDataValue(tr("Artist"), AudioPlayerMetaData::artist());
        metaData << ItemMetaDataValue(tr("Genre"), AudioPlayerMetaData::genre());
        metaData << ItemMetaDataValue(tr("Duration"), AudioPlayerMetaData::durationString());
        m_nowPlayingMetaData->setThumbnailSource(AudioPlayerMetaData::largeThumbnailUrl());
        m_nowPlayingMetaData->setMetaData(metaData);
        m_nowPlayingMetaData->show();
        setWindowTitle(QString("MusiKloud2 - %1 - %2").arg(AudioPlayerMetaData::artist())
                       .arg(AudioPlayerMetaData::title()));
    }
    else {
        m_nowPlayingMetaData->hide();
        m_nowPlayingMetaData->clear();
        setWindowTitle("MusiKloud2");
    }
}

void MainWindow::updatePlayerPosition() {
    AudioPlayer::instance()->setPosition(qint64(m_positionSlider->value()));
}

void MainWindow::onPlayerDurationChanged(qint64 duration) {
    m_positionSlider->setMaximum(int(duration));
    m_positionLabel->setText(QString("%1 / %2").arg(AudioPlayer::instance()->positionString())
                             .arg(AudioPlayer::instance()->durationString()));    
}

void MainWindow::onPlayerPositionChanged(qint64 position) {
    if (m_positionSlider->isEnabled()) {
        if (!m_positionSlider->isSliderDown()) {
            m_positionSlider->setValue(int(position));
        }
    }
    else {
        m_positionLabel->setText(QString("%1 / %2").arg(AudioPlayer::instance()->positionString())
                                 .arg(AudioPlayer::instance()->durationString()));
    }
}

void MainWindow::onPlayerSeekableChanged(bool seekable) {
    m_positionSpacerAction->setVisible(!seekable);
    m_positionSliderAction->setVisible(seekable);
}

void MainWindow::onPlayerStatusChanged(AudioPlayer::Status status) {
    switch (status) {
    case AudioPlayer::Stopped:
        m_playPauseAction->setIcon(QIcon::fromTheme("media-playback-start"));
        m_playPauseAction->setText(tr("Play"));
        m_positionSpacerAction->setVisible(true);
        m_positionSliderAction->setVisible(false);
        m_positionLabelAction->setVisible(false);
        break;
    case AudioPlayer::Paused:
        m_playPauseAction->setIcon(QIcon::fromTheme("media-playback-start"));
        m_playPauseAction->setText(tr("Play"));
        break;
    case AudioPlayer::Playing:
        m_playPauseAction->setIcon(QIcon::fromTheme("media-playback-pause"));
        m_playPauseAction->setText(tr("Pause"));
        m_positionLabelAction->setVisible(true);
        break;
    case AudioPlayer::Failed:
        m_playPauseAction->setIcon(QIcon::fromTheme("media-playback-start"));
        m_playPauseAction->setText(tr("Play"));
        m_positionSpacerAction->setVisible(true);
        m_positionSliderAction->setVisible(false);
        m_positionLabelAction->setVisible(false);
        QMessageBox::critical(this, tr("Playback error"), AudioPlayer::instance()->errorString());
        break;
    default:
        m_playPauseAction->setIcon(QIcon::fromTheme("media-playback-start"));
        m_playPauseAction->setText(tr("Play"));
        break;
    }
}

void MainWindow::onSliderValueChanged(int value) {
    m_positionLabel->setText(QString("%1 / %2").arg(Utils::formatMSecs(qint64(value)))
                             .arg(AudioPlayer::instance()->durationString()));
}
