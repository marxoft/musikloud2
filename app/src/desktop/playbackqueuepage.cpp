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

#include "playbackqueuepage.h"
#include "audioplayer.h"
#include "plugindownloaddialog.h"
#include "resources.h"
#include "settings.h"
#include "soundclouddownloaddialog.h"
#include "transfers.h"
#include "treeview.h"
#include <QDropEvent>
#include <QHeaderView>
#include <QMenu>
#include <QMimeData>
#include <QVBoxLayout>

PlaybackQueuePage::PlaybackQueuePage(QWidget *parent) :
    Page(parent),
    m_view(new MKTreeView(this)),
    m_layout(new QVBoxLayout(this))
{
    setAcceptDrops(true);
    updateWindowTitle();
    
    m_view->setModel(AudioPlayer::instance()->queue());
    m_view->setAlternatingRowColors(true);
    m_view->setSelectionBehavior(QTreeView::SelectRows);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setEditTriggers(QTreeView::NoEditTriggers);
    m_view->setItemsExpandable(false);
    m_view->setUniformRowHeights(true);
    m_view->setAllColumnsShowFocus(true);
    m_view->setRootIsDecorated(false);
    m_view->header()->restoreState(Settings::tracksHeaderViewState());
    m_view->setItemMetaDataEnabled(true);
    m_view->setItemMetaDataThumbnailRole(TrackModel::LargeThumbnailUrlRole);
    
    QList<ItemMetaDataRole> roles;
    roles << ItemMetaDataRole(tr("Title"), TrackModel::TitleRole);
    roles << ItemMetaDataRole(tr("Artist"), TrackModel::ArtistRole);
    roles << ItemMetaDataRole(tr("Genre"), TrackModel::GenreRole);
    roles << ItemMetaDataRole(tr("Duration"), TrackModel::DurationStringRole);
    m_view->setItemMetaDataTextRoles(roles);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(playTrack(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(AudioPlayer::instance(), SIGNAL(queueCountChanged(int)), this, SLOT(updateWindowTitle()));
    connect(AudioPlayer::instance(), SIGNAL(currentIndexChanged(int)), this, SLOT(updateWindowTitle()));
}

void PlaybackQueuePage::closeEvent(QCloseEvent *e) {
    Settings::setTracksHeaderViewState(m_view->header()->saveState());
    Page::closeEvent(e);
}

void PlaybackQueuePage::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void PlaybackQueuePage::dropEvent(QDropEvent *e) {
    if (e->mimeData()->hasUrls()) {
        AudioPlayer::instance()->addUrls(e->mimeData()->urls());
    }
}

void PlaybackQueuePage::downloadTrack(const QModelIndex &index) {
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

void PlaybackQueuePage::playTrack(const QModelIndex &index) {
    AudioPlayer::instance()->setCurrentIndex(index.row(), true);
}

void PlaybackQueuePage::removeTrack(const QModelIndex &index) {
    AudioPlayer::instance()->removeTrack(index.row());
}

void PlaybackQueuePage::showContextMenu(const QPoint &pos) {
    if (!m_view->currentIndex().isValid()) {
        return;
    }
    
    if (m_view->currentIndex().data(TrackModel::DownloadableRole).toBool()) {
        QMenu menu(this);
        QAction *playAction = menu.addAction(QIcon::fromTheme("media-playback-start"), tr("&Play"));
        QAction *downloadAction = menu.addAction(QIcon::fromTheme("document-save"), tr("&Download"));
        QAction *removeAction = menu.addAction(QIcon::fromTheme("list-remove"), tr("&Remove"));
        QAction *action = menu.exec(m_view->mapToGlobal(pos));
    
        if (action == playAction) {
            playTrack(m_view->currentIndex());
        }
        else if (action == downloadAction) {
            downloadTrack(m_view->currentIndex());
        }
        else if (action == removeAction) {
            removeTrack(m_view->currentIndex());
        }
    }
    else {
        QMenu menu(this);
        QAction *playAction = menu.addAction(QIcon::fromTheme("media-playback-start"), tr("&Play"));
        QAction *removeAction = menu.addAction(QIcon::fromTheme("list-remove"), tr("&Remove"));
        QAction *action = menu.exec(m_view->mapToGlobal(pos));
    
        if (action == playAction) {
            playTrack(m_view->currentIndex());
        }
        else if (action == removeAction) {
            removeTrack(m_view->currentIndex());
        }
    }
}

void PlaybackQueuePage::updateWindowTitle() {
    if (AudioPlayer::instance()->queueCount() > 0) {
        if (AudioPlayer::instance()->currentIndex() >= 0) {
            setWindowTitle(tr("Playback queue (%1/%2)").arg(AudioPlayer::instance()->currentIndex() + 1)
                           .arg(AudioPlayer::instance()->queueCount()));
        }
        else {
            setWindowTitle(tr("Playback queue (%1)").arg(AudioPlayer::instance()->queueCount()));
        }
    }
    else {
        setWindowTitle(tr("Playback queue"));
    }
}
