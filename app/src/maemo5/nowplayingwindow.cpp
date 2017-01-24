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

#include "nowplayingwindow.h"
#include "image.h"
#include "nowplayingdelegate.h"
#include "plugindownloaddialog.h"
#include "resources.h"
#include "soundclouddownloaddialog.h"
#include "screen.h"
#include "transfers.h"
#include "utils.h"
#include <QLabel>
#include <QSlider>
#include <QProgressBar>
#include <QListView>
#include <QToolButton>
#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
#include <QAction>

const QString NowPlayingWindow::TOOL_BUTTON_STYLE_SHEET("QToolButton { background: transparent; image: url(%1); } \
                                                         QToolButton:pressed { image: url(%2); } \
                                                         QToolButton:checked { image: url(%3); }");

NowPlayingWindow::NowPlayingWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_thumbnail(new Image(this)),
    m_container(new QWidget(this)),
    m_grid(new QGridLayout(m_container)),
    m_trackLabel(new QLabel(this)),
    m_titleLabel(new QLabel(this)),
    m_artistLabel(new QLabel(this)),
    m_positionLabel(new QLabel(this)),
    m_durationLabel(new QLabel(this)),
    m_positionSlider(new QSlider(Qt::Horizontal, this)),
    m_bufferBar(new QProgressBar(this)),
    m_view(new QListView(this)),
    m_contextMenu(new QMenu(this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_removeAction(new QAction(tr("Delete from now playing"), this)),
    m_clearAction(new QAction(tr("Clear now playing"), this)),
    m_stopAfterCurrentAction(new QAction(tr("Stop after current track"), this)),
    m_toolBar(new QWidget(this)),
    m_hbox(new QHBoxLayout(m_toolBar)),
    m_previousButton(new QToolButton(this)),
    m_playButton(new QToolButton(this)),
    m_nextButton(new QToolButton(this)),
    m_shuffleButton(new QToolButton(this)),
    m_repeatButton(new QToolButton(this))
{
    setWindowTitle(tr("Now playing"));
    
    menuBar()->addAction(m_clearAction);
    menuBar()->addAction(m_stopAfterCurrentAction);
    
    m_thumbnail->setFallbackSource(QUrl::fromLocalFile("/usr/share/icons/hicolor/295x295/hildon/mediaplayer_default_album.png"));
    m_thumbnail->setGeometry(30, 30, 295, 295);
    m_container->setGeometry(345, 0, 445, 354);
    m_view->setGeometry(345, 0, 445, 354);
    m_toolBar->setGeometry(15, 354, 770, 70);
    
    m_trackLabel->setStyleSheet(QString("color: %1; font-size: 13pt").arg(palette().color(QPalette::Mid).name()));
    m_positionLabel->setStyleSheet("font-size: 13pt");
    m_durationLabel->setStyleSheet("font-size: 13pt");
    m_trackLabel->setFixedHeight(60);
    m_titleLabel->setFixedHeight(60);
    m_positionLabel->setFixedSize(50, 70);
    m_durationLabel->setFixedSize(50, 70);
    m_positionLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_durationLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    m_positionSlider->setFixedHeight(70);
    m_bufferBar->setFixedHeight(70);
    m_bufferBar->setMaximum(100);
    m_bufferBar->setTextVisible(true);
    
    m_view->setModel(AudioPlayer::instance()->queue());
    m_view->setItemDelegate(new NowPlayingDelegate(m_view));
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->hide();
    
    m_contextMenu->addAction(m_downloadAction);
    m_contextMenu->addAction(m_removeAction);
    m_contextMenu->addAction(m_clearAction);
    
    m_stopAfterCurrentAction->setCheckable(true);
    m_stopAfterCurrentAction->setChecked(AudioPlayer::instance()->stopAfterCurrentTrack());
        
    m_previousButton->setFixedSize(64, 64);
    m_previousButton->setIconSize(QSize(64, 64));
    m_previousButton->setShortcut(Qt::Key_Left);
    m_previousButton->setStyleSheet(TOOL_BUTTON_STYLE_SHEET.arg(QString("/etc/hildon/theme/mediaplayer/Back.png"))
                                                           .arg(QString("/etc/hildon/theme/mediaplayer/BackPressed.png"))
                                                           .arg(QString("/etc/hildon/theme/mediaplayer/Back.png")));
    
    m_playButton->setFixedSize(64, 64);
    m_playButton->setIconSize(QSize(64, 64));
    m_playButton->setCheckable(true);
    m_playButton->setShortcut(Qt::Key_Space);
    
    m_nextButton->setFixedSize(64, 64);
    m_nextButton->setIconSize(QSize(64, 64));
    m_nextButton->setShortcut(Qt::Key_Right);
    m_nextButton->setStyleSheet(TOOL_BUTTON_STYLE_SHEET.arg(QString("/etc/hildon/theme/mediaplayer/Forward.png"))
                                                       .arg(QString("/etc/hildon/theme/mediaplayer/ForwardPressed.png"))
                                                       .arg(QString("/etc/hildon/theme/mediaplayer/Forward.png")));
    
    m_shuffleButton->setFixedSize(64, 64);
    m_shuffleButton->setIconSize(QSize(64, 64));
    m_shuffleButton->setCheckable(true);
    m_shuffleButton->setShortcut(Qt::Key_S);
    m_shuffleButton->setStyleSheet(TOOL_BUTTON_STYLE_SHEET.arg(QString("/etc/hildon/theme/mediaplayer/Shuffle.png"))
                                                          .arg(QString("/etc/hildon/theme/mediaplayer/ShufflePressed.png"))
                                                          .arg(QString("/etc/hildon/theme/mediaplayer/ShufflePressed.png")));
    
    m_repeatButton->setFixedSize(64, 64);
    m_repeatButton->setIconSize(QSize(64, 64));
    m_repeatButton->setCheckable(true);
    m_repeatButton->setShortcut(Qt::Key_R);
    m_repeatButton->setStyleSheet(TOOL_BUTTON_STYLE_SHEET.arg(QString("/etc/hildon/theme/mediaplayer/Repeat.png"))
                                                         .arg(QString("/etc/hildon/theme/mediaplayer/RepeatPressed.png"))
                                                         .arg(QString("/etc/hildon/theme/mediaplayer/RepeatPressed.png")));
        
    m_grid->addWidget(m_trackLabel, 0, 0, 1, 3);
    m_grid->addWidget(m_titleLabel, 1, 0, 1, 3);
    m_grid->addWidget(m_positionLabel, 2, 0);
    m_grid->addWidget(m_positionSlider, 2, 1);
    m_grid->addWidget(m_bufferBar, 2, 1);
    m_grid->addWidget(m_durationLabel, 2, 2);
    m_grid->addWidget(m_artistLabel, 4, 0, 1, 3);
    m_grid->setColumnStretch(1, 1);
    
    m_hbox->setContentsMargins(0, 0, 0, 0);
    m_hbox->setSpacing(67);
    m_hbox->addWidget(m_previousButton);
    m_hbox->addWidget(m_playButton);
    m_hbox->addWidget(m_nextButton);
    m_hbox->addStretch(1);
    m_hbox->addWidget(m_shuffleButton);
    m_hbox->addWidget(m_repeatButton);
    
    connect(m_thumbnail, SIGNAL(clicked()), this, SLOT(toggleView()));
    connect(m_positionSlider, SIGNAL(sliderReleased()), this, SLOT(setPosition()));
    connect(m_positionSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(setCurrentIndex(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadTrack()));
    connect(m_removeAction, SIGNAL(triggered()), this, SLOT(removeTrack()));
    connect(m_clearAction, SIGNAL(triggered()), AudioPlayer::instance(), SLOT(clearQueue()));
    connect(m_stopAfterCurrentAction, SIGNAL(triggered(bool)),
            AudioPlayer::instance(), SLOT(setStopAfterCurrentTrack(bool)));
    connect(m_previousButton, SIGNAL(clicked()), AudioPlayer::instance(), SLOT(previous()));
    connect(m_playButton, SIGNAL(clicked(bool)), AudioPlayer::instance(), SLOT(setPlaying(bool)));
    connect(m_nextButton, SIGNAL(clicked()), AudioPlayer::instance(), SLOT(next()));
    connect(m_shuffleButton, SIGNAL(clicked(bool)), AudioPlayer::instance(), SLOT(setShuffleEnabled(bool)));
    connect(m_repeatButton, SIGNAL(clicked(bool)), AudioPlayer::instance(), SLOT(setRepeatEnabled(bool)));
    connect(AudioPlayer::instance(), SIGNAL(queueCountChanged(int)), this, SLOT(onCountChanged(int)));
    connect(Screen::instance(), SIGNAL(screenLockStateChanged(bool)), this, SLOT(onScreenLockStateChanged(bool)));
    
    connectPlaybackSignals();
}

void NowPlayingWindow::connectPlaybackSignals() {
    connect(AudioPlayer::instance(), SIGNAL(bufferStatusChanged(int)), this, SLOT(onBufferStatusChanged(int)));
    connect(AudioPlayer::instance(), SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    connect(AudioPlayer::instance(), SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));
    connect(AudioPlayer::instance(), SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
    connect(AudioPlayer::instance(), SIGNAL(metaDataChanged()), this, SLOT(onMetaDataChanged()));
    connect(AudioPlayer::instance(), SIGNAL(seekableChanged(bool)), this, SLOT(onSeekableChanged(bool)));
    connect(AudioPlayer::instance(), SIGNAL(statusChanged(AudioPlayer::Status)),
            this, SLOT(onStatusChanged(AudioPlayer::Status)));
    
    onCurrentIndexChanged(AudioPlayer::instance()->currentIndex());
    onDurationChanged(AudioPlayer::instance()->duration());
    onMetaDataChanged();
    onPositionChanged(AudioPlayer::instance()->position());
    onSeekableChanged(AudioPlayer::instance()->isSeekable());
    onStatusChanged(AudioPlayer::instance()->status());
}

void NowPlayingWindow::disconnectPlaybackSignals() {
    disconnect(AudioPlayer::instance(), SIGNAL(bufferStatusChanged(int)), this, SLOT(onBufferStatusChanged(int)));
    disconnect(AudioPlayer::instance(), SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    disconnect(AudioPlayer::instance(), SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));
    disconnect(AudioPlayer::instance(), SIGNAL(metaDataChanged()), this, SLOT(onMetaDataChanged()));
    disconnect(AudioPlayer::instance(), SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
    disconnect(AudioPlayer::instance(), SIGNAL(seekableChanged(bool)), this, SLOT(onSeekableChanged(bool)));
    disconnect(AudioPlayer::instance(), SIGNAL(statusChanged(AudioPlayer::Status)),
               this, SLOT(onStatusChanged(AudioPlayer::Status)));
}

void NowPlayingWindow::downloadTrack() {
    const QModelIndex index = m_view->currentIndex();
    const QString trackId = index.data(TrackModel::IdRole).toString();
    const QString title = index.data(TrackModel::TitleRole).toString();
    const QString service = index.data(TrackModel::ServiceRole).toString();
    
    if (service == Resources::SOUNDCLOUD) {
        SoundCloudDownloadDialog dialog(this);
        dialog.get(trackId);

        if (dialog.exec() == QDialog::Accepted) {
            Transfers::instance()->addDownloadTransfer(Resources::SOUNDCLOUD, trackId, dialog.streamId(),
                                                       QUrl(), title, dialog.category(), dialog.customCommand(),
                                                       dialog.customCommandOverrideEnabled());
        }
    }
    else {
        const QUrl streamUrl = index.data(TrackModel::StreamUrlRole).toString();
        PluginDownloadDialog dialog(service, this);
        dialog.list(trackId, streamUrl.isEmpty());
        
        if (dialog.exec() == QDialog::Accepted) {
            Transfers::instance()->addDownloadTransfer(service, trackId, dialog.streamId(), streamUrl,
                                                       title, dialog.category(), dialog.customCommand(),
                                                       dialog.customCommandOverrideEnabled());
        }
    }
}

void NowPlayingWindow::removeTrack() {
    AudioPlayer::instance()->removeTrack(m_view->currentIndex().row());
}

void NowPlayingWindow::setCurrentIndex(const QModelIndex &index) {
    AudioPlayer::instance()->setCurrentIndex(index.row(), AudioPlayer::instance()->isPlaying());
}

void NowPlayingWindow::setPosition() {
    AudioPlayer::instance()->setPosition(m_positionSlider->value());
}

void NowPlayingWindow::showContextMenu(const QPoint &pos) {
    if (m_view->currentIndex().isValid()) {
        m_downloadAction->setEnabled(m_view->currentIndex().data(TrackModel::DownloadableRole).toBool());
        m_contextMenu->popup(pos, m_downloadAction);
    }
}

void NowPlayingWindow::toggleView() {
    if (m_view->isVisible()) {
        m_container->show();
        m_view->hide();
    }
    else {
        m_view->setCurrentIndex(AudioPlayer::instance()->queue()->index(AudioPlayer::instance()->currentIndex()));
        m_view->scrollTo(m_view->currentIndex(), QListView::PositionAtCenter);
        m_view->show();
        m_view->setFocus(Qt::OtherFocusReason);
        m_container->hide();
    }
}

void NowPlayingWindow::onBufferStatusChanged(int progress) {
    m_bufferBar->setValue(progress);
}

void NowPlayingWindow::onCountChanged(int count) {
    if (count > 0) {
        m_trackLabel->setText(QString("%1/%2 %3").arg(AudioPlayer::instance()->currentIndex() + 1).arg(count)
                                             .arg(tr("tracks")));
    }
    else {
        close();
    }
}

void NowPlayingWindow::onCurrentIndexChanged(int index) {
    if (m_view->isVisible()) {
        m_view->setCurrentIndex(AudioPlayer::instance()->queue()->index(index));
        m_view->scrollTo(m_view->currentIndex(), QListView::PositionAtCenter);
    }

    m_trackLabel->setText(QString("%1/%2 %3").arg(index + 1).arg(AudioPlayer::instance()->queueCount())
                                             .arg(tr("tracks")));
}

void NowPlayingWindow::onDurationChanged(qint64 duration) {
    m_positionSlider->setMaximum(int(duration));
    m_durationLabel->setText(AudioPlayerMetaData::durationString());
}

void NowPlayingWindow::onMetaDataChanged() {
    m_titleLabel->setText(m_titleLabel->fontMetrics().elidedText(AudioPlayerMetaData::title(),
                                                                 Qt::ElideRight, 415));
    const QString artist = m_artistLabel->fontMetrics().elidedText(AudioPlayerMetaData::artist(),
                                                                   Qt::ElideRight, 415);
    const QString genre = m_artistLabel->fontMetrics().elidedText(AudioPlayerMetaData::genre(),
                                                                  Qt::ElideRight, 415);
    m_durationLabel->setText(AudioPlayerMetaData::durationString());
    m_artistLabel->setText(QString("<p>%1<br><font color='%2'>%3</font></p>").arg(artist)
                           .arg(palette().color(QPalette::Mid).name()).arg(genre));
    m_thumbnail->setSource(AudioPlayerMetaData::largeThumbnailUrl());
}

void NowPlayingWindow::onPositionChanged(qint64 position) {
    if (m_positionSlider->isEnabled()) {
        if (!m_positionSlider->isSliderDown()) {
            m_positionSlider->setValue(int(position));
        }
    }
    else {
        m_positionLabel->setText(AudioPlayer::instance()->positionString());
    }
}

void NowPlayingWindow::onSeekableChanged(bool isSeekable) {
    if (isSeekable) {
        m_positionSlider->setEnabled(true);
        
        m_playButton->setStyleSheet(TOOL_BUTTON_STYLE_SHEET.arg(QString("/etc/hildon/theme/mediaplayer/Play.png"))
                                                           .arg(QString("/etc/hildon/theme/mediaplayer/Play.png"))
                                                           .arg(QString("/etc/hildon/theme/mediaplayer/Pause.png")));
    }
    else {
        m_positionSlider->setEnabled(false);
        m_positionSlider->setValue(0);
        
        m_playButton->setStyleSheet(TOOL_BUTTON_STYLE_SHEET.arg(QString("/etc/hildon/theme/mediaplayer/Play.png"))
                                                           .arg(QString("/etc/hildon/theme/mediaplayer/Play.png"))
                                                           .arg(QString("/etc/hildon/theme/mediaplayer/Stop.png")));
    }
}

void NowPlayingWindow::onSliderValueChanged(int value) {
    m_positionLabel->setText(Utils::formatMSecs(qint64(value)));
}

void NowPlayingWindow::onStatusChanged(AudioPlayer::Status status) {
    switch (status) {
    case AudioPlayer::Stopped:
    case AudioPlayer::Failed:
        m_playButton->setChecked(false);
        m_positionSlider->setEnabled(false);
        m_positionSlider->setValue(0);
        break;
    case AudioPlayer::Loading:
        m_bufferBar->setFormat(tr("Loading"));
        m_bufferBar->setValue(100);
        m_positionSlider->hide();
        m_bufferBar->show();
        return;
    case AudioPlayer::Buffering:
        m_bufferBar->setFormat(tr("Buffering") + " %p%");
        m_positionSlider->hide();
        m_bufferBar->show();
        return;
    case AudioPlayer::Playing:
        m_playButton->setChecked(true);
        m_positionSlider->setEnabled(AudioPlayer::instance()->isSeekable());
        break;
    default:
        m_playButton->setChecked(false);
        m_positionSlider->setEnabled(AudioPlayer::instance()->isSeekable());
        break;
    }
    
    m_bufferBar->hide();
    m_positionSlider->show();
}

void NowPlayingWindow::onScreenLockStateChanged(bool isLocked) {
    if (isLocked) {
        disconnectPlaybackSignals();
    }
    else {
        connectPlaybackSignals();
    }
}
