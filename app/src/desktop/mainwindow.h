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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "audioplayer.h"
#include <QMainWindow>

class ItemMetaData;
class QLabel;
class QSlider;
class QTabWidget;
class QToolButton;
class QVBoxLayout;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private Q_SLOTS:
    void addFiles();
    void addUrl();
    void openFiles();
    void openUrl();
    void search();
    
    void reloadTab(int index);
    void reloadCurrentTab();
    void closeTab(int index);
    void closeCurrentTab();
    void updateTabText(const QString &text);
    void onTabStatusChanged();
    
    void loadPlugins();
    
    void showAboutDialog();
    void showPlaybackQueue();
    void showSettingsDialog();
    void showTransfers();
    
    void updateNowPlayingMetaData();
    
    void updatePlayerPosition();
    void onPlayerDurationChanged(qint64 duration);
    void onPlayerPositionChanged(qint64 position);
    void onPlayerSeekableChanged(bool seekable);
    void onPlayerStatusChanged(AudioPlayer::Status status);
    
    void onSliderValueChanged(int value);
    
private:
    virtual void closeEvent(QCloseEvent *event);
    
    QMenu *m_fileMenu;
    QMenu *m_viewMenu;
    QMenu *m_playbackMenu;
    QMenu *m_transfersMenu;
    QMenu *m_editMenu;
    QMenu *m_helpMenu;
    
    QAction *m_openFilesAction;
    QAction *m_openUrlAction;
    QAction *m_addFilesAction;
    QAction *m_addUrlAction;
    QAction *m_searchAction;
    QAction *m_pluginsAction;
    QAction *m_quitAction;
    
    QAction *m_showPlaybackQueueAction;
    QAction *m_showTransfersAction;
    QAction *m_reloadAction;
    QAction *m_closeAction;
    
    QAction *m_playPauseAction;
    QAction *m_stopAction;
    QAction *m_previousAction;
    QAction *m_nextAction;
    QAction *m_repeatAction;
    QAction *m_shuffleAction;
    QAction *m_stopAfterCurrentAction;
    QAction *m_clearAction;
    
    QAction *m_startTransfersAction;
    QAction *m_pauseTransfersAction;

    QAction *m_settingsAction;

    QAction *m_aboutAction;
    
    QWidget *m_positionSpacer;
    QAction *m_positionSpacerAction;
    
    QSlider *m_positionSlider;
    QAction *m_positionSliderAction;
    
    QLabel *m_positionLabel;
    QAction *m_positionLabelAction;
    
    QToolBar *m_toolBar;
    
    QWidget *m_widget;
    
    QTabWidget *m_tabWidget;
    
    ItemMetaData *m_nowPlayingMetaData;
    
    QVBoxLayout *m_layout;
};

#endif // MAINWINDOW_H
