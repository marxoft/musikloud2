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
#include "settings.h"
#include "soundcloud.h"
#include "soundclouddownloaddialog.h"
#include "soundcloudtrackwindow.h"
#include "trackdelegate.h"
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
    m_contextMenu(new QMenu(this)),
    m_queueAction(new QAction(tr("Queue"), this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_favouriteAction(0),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No tracks found")), this))
{
    setWindowTitle(tr("Tracks"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    m_reloadAction->setEnabled(false);
    
    m_contextMenu->addAction(m_queueAction);
    m_contextMenu->addAction(m_downloadAction);  
    m_contextMenu->addAction(m_shareAction);  
    
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
    connect(m_queueAction, SIGNAL(triggered()), this, SLOT(queueTrack()));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadTrack()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareTrack()));
    
    if (!SoundCloud::instance()->userId().isEmpty()) {
        m_favouriteAction = new QAction(this);
        m_contextMenu->addAction(m_favouriteAction);
        connect(m_favouriteAction, SIGNAL(triggered()), this, SLOT(setTrackFavourite()));
    }
}

SoundCloudTracksWindow::~SoundCloudTracksWindow() {
    delete m_cache;
    m_cache = 0;
}

void SoundCloudTracksWindow::get(const QString &resourcePath, const QVariantMap &filters) {
    m_model->get(resourcePath, filters);
}

void SoundCloudTracksWindow::downloadTrack() {
    if (isBusy()) {
        return;
    }
    
    if (m_view->currentIndex().isValid()) {
        QString id = m_view->currentIndex().data(SoundCloudTrackModel::IdRole).toString();
        QString title = m_view->currentIndex().data(SoundCloudTrackModel::TitleRole).toString();
        
        SoundCloudDownloadDialog *dialog = new SoundCloudDownloadDialog(id, title, this);
        dialog->open();
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

void SoundCloudTracksWindow::queueTrack() {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_model->get(m_view->currentIndex().row())) {
        AudioPlayer::instance()->addTrack(track);
        QMaemo5InformationBox::information(this, tr("'%1' added to playback queue").arg(track->title()));
    }
}

void SoundCloudTracksWindow::setTrackFavourite() {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_model->get(m_view->currentIndex().row())) {
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

void SoundCloudTracksWindow::shareTrack() {
    if (const SoundCloudTrack *track = m_model->get(m_view->currentIndex().row())) {
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
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        if (m_favouriteAction) {
            m_favouriteAction->setText(m_view->currentIndex().data(SoundCloudTrackModel::FavouriteRole).toBool()
                                       ? tr("Unfavourite") : tr("Favourite"));
        }
        
        m_contextMenu->popup(pos, m_queueAction);
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
