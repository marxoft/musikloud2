/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef NOWPLAYINGWINDOW_H
#define NOWPLAYINGWINDOW_H

#include "stackedwindow.h"
#include "audioplayer.h"

class Image;
class QToolButton;
class QSlider;
class QProgressBar;
class QListView;
class QLabel;
class QAction;
class QMenu;
class QHBoxLayout;
class QGridLayout;

class NowPlayingWindow : public StackedWindow
{
    Q_OBJECT

public:
    explicit NowPlayingWindow(StackedWindow *parent = 0);
    
private Q_SLOTS:
    void removeTrack();
    
    void setCurrentIndex(const QModelIndex &index);
    
    void setPosition();
    
    void showContextMenu(const QPoint &pos);
    
    void toggleView();
    
    void onBufferStatusChanged(int progress);
    void onCountChanged(int count);
    void onCurrentIndexChanged(int index);
    void onDurationChanged(qint64 duration);
    void onPositionChanged(qint64 position);
    void onSeekableChanged(bool isSeekable);
    void onStatusChanged(AudioPlayer::Status status);
    void onScreenLockStateChanged(bool isLocked);

private:
    void connectPlaybackSignals();
    void disconnectPlaybackSignals();
    
    Image *m_thumbnail;
    QWidget *m_container;
    QGridLayout *m_grid;
    QLabel *m_trackLabel;
    QLabel *m_titleLabel;
    QLabel *m_artistLabel;
    QLabel *m_positionLabel;
    QLabel *m_durationLabel;
    QSlider *m_positionSlider;
    QProgressBar *m_bufferBar;
    QListView *m_view;
    QMenu *m_contextMenu;
    QAction *m_removeAction;
    QAction *m_clearAction;
    QWidget *m_toolBar;
    QHBoxLayout *m_hbox;
    QToolButton *m_previousButton;
    QToolButton *m_playButton;
    QToolButton *m_nextButton;
    QToolButton *m_shuffleButton;
    QToolButton *m_repeatButton;
};

#endif // NOWPLAYINGWINDOW_H
