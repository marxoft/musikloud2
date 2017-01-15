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

#include "soundcloudtrackswindow.h"
#include "audioplayer.h"
#include "clipboard.h"
#include "imagecache.h"
#include "listview.h"
#include "nowplayingaction.h"
#include "nowplayingwindow.h"
#include "resources.h"
#include "soundcloud.h"
#include "soundclouddownloaddialog.h"
#include "soundcloudtrackwindow.h"
#include "trackdelegate.h"
#include "transfers.h"
#include <qsoundcloud/urls.h>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QMaemo5InformationBox>

SoundCloudTracksWindow::SoundCloudTracksWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new SoundCloudTrackModel(this)),
    m_cache(new ImageCache),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_delegate(new TrackDelegate(m_cache, SoundCloudTrackModel::ArtistRole, SoundCloudTrackModel::DateRole,
                                 SoundCloudTrackModel::DurationStringRole, SoundCloudTrackModel::ThumbnailUrlRole,
                                 SoundCloudTrackModel::TitleRole, this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No results")), this))
{
    setWindowTitle(tr("Tracks"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    m_reloadAction->setEnabled(false); 
    
    m_label->hide();
    
    m_layout = new QVBoxLayout(centralWidget());
    m_layout->addWidget(m_view);
    m_layout->addWidget(m_label);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    menuBar()->addAction(m_reloadAction);
    menuBar()->addAction(m_nowPlayingAction);
    
    connect(m_model, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(QSoundCloud::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showTrack(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playTrack(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
}

SoundCloudTracksWindow::~SoundCloudTracksWindow() {
    delete m_cache;
    m_cache = 0;
}

void SoundCloudTracksWindow::get(const QString &resourcePath, const QVariantMap &filters) {
    m_model->get(resourcePath, filters);
}

void SoundCloudTracksWindow::downloadTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
        
    if (index.isValid()) {
        const QString id = index.data(SoundCloudTrackModel::IdRole).toString();
        const QString title = index.data(SoundCloudTrackModel::TitleRole).toString();
        
        SoundCloudDownloadDialog dialog(this);
        dialog.get(id);

        if (dialog.exec() == QDialog::Accepted) {
            Transfers::instance()->addDownloadTransfer(Resources::SOUNDCLOUD, id, dialog.streamId(), QUrl(), title,
                                                       dialog.category(), dialog.customCommand(),
                                                       dialog.customCommandOverrideEnabled());
        }
    }
}

void SoundCloudTracksWindow::playTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_model->get(index.row())) {
        AudioPlayer::instance()->playTrack(track);
        NowPlayingWindow *window = new NowPlayingWindow(this);
        window->show();
    }
}

void SoundCloudTracksWindow::queueTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_model->get(index.row())) {
        AudioPlayer::instance()->addTrack(track);
        QMaemo5InformationBox::information(this, tr("'%1' added to playback queue").arg(track->title()));
    }
}

void SoundCloudTracksWindow::setTrackFavourite(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_model->get(index.row())) {
        connect(track, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
                this, SLOT(onTrackUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status)));
        
        if (track->isFavourite()) {
            track->unfavourite();
        }
        else {
            track->favourite();
        }
    }
}

void SoundCloudTracksWindow::shareTrack(const QModelIndex &index) {
    if (const SoundCloudTrack *track = m_model->get(index.row())) {
        Clipboard::instance()->setText(track->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void SoundCloudTracksWindow::showTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_model->get(index.row())) {
        SoundCloudTrackWindow *window = new SoundCloudTrackWindow(track, this);
        window->show();
    }
}

void SoundCloudTracksWindow::showContextMenu(const QPoint &pos) {
    if (isBusy()) {
        return;
    }
    
    const QModelIndex index = m_view->currentIndex();
    
    if (!index.isValid()) {
        return;
    }
    
    if (!SoundCloud::userId().isEmpty()) {
        QMenu menu(this);
        QAction *queueAction = menu.addAction(tr("Queue"));
        QAction *downloadAction = menu.addAction(tr("Download"));
        QAction *shareAction = menu.addAction(tr("Copy URL"));
        QAction *favouriteAction = menu.addAction(index.data(SoundCloudTrackModel::FavouriteRole).toBool()
                                                  ? tr("Unfavourite") : tr("Favourite"));
        QAction *action = menu.exec(pos);
        
        if (!action) {
            return;
        }
        
        if (action == queueAction) {
            queueTrack(index);
        }
        else if (action == downloadAction) {
            downloadTrack(index);
        }
        else if (action == shareAction) {
            shareTrack(index);
        }
        else if (action == favouriteAction) {
            setTrackFavourite(index);
        }
    }
    else {
        QMenu menu(this);
        QAction *queueAction = menu.addAction(tr("Queue"));
        QAction *downloadAction = menu.addAction(tr("Download"));
        QAction *shareAction = menu.addAction(tr("Copy URL"));
        QAction *action = menu.exec(pos);
        
        if (!action) {
            return;
        }
        
        if (action == queueAction) {
            queueTrack(index);
        }
        else if (action == downloadAction) {
            downloadTrack(index);
        }
        else if (action == shareAction) {
            shareTrack(index);
        }
    }
}

void SoundCloudTracksWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void SoundCloudTracksWindow::onModelStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
    switch (status) {
    case QSoundCloud::ResourcesRequest::Loading:
        showProgressIndicator();
        m_label->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case QSoundCloud::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_model->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    m_reloadAction->setEnabled(true);
    
    if (m_model->rowCount() == 0) {
        m_view->hide();
        m_label->show();
    }
}

void SoundCloudTracksWindow::onTrackUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
    const SoundCloudTrack *track = qobject_cast<SoundCloudTrack*>(sender());
    
    if (!track) {
        return;
    }
    
    switch (status) {
    case QSoundCloud::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QSoundCloud::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), track->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(track, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
               this, SLOT(onTrackUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status)));
}
