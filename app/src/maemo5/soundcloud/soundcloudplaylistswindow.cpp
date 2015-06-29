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

#include "soundcloudplaylistswindow.h"
#include "imagecache.h"
#include "listview.h"
#include "nowplayingaction.h"
#include "playlistdelegate.h"
#include "settings.h"
#include "soundcloudplaylistwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>

SoundCloudPlaylistsWindow::SoundCloudPlaylistsWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new SoundCloudPlaylistModel(this)),
    m_cache(new ImageCache),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No sets found")), this))
{
    setWindowTitle(tr("Sets"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(new PlaylistDelegate(m_cache, SoundCloudPlaylistModel::ArtistRole,
                                                 SoundCloudPlaylistModel::DateRole,
                                                 SoundCloudPlaylistModel::ThumbnailUrlRole,
                                                 SoundCloudPlaylistModel::TitleRole,
                                                 SoundCloudPlaylistModel::TrackCountRole, m_view));
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
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showPlaylist(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
}

SoundCloudPlaylistsWindow::~SoundCloudPlaylistsWindow() {
    delete m_cache;
    m_cache = 0;
}

void SoundCloudPlaylistsWindow::get(const QString &resourcePath, const QVariantMap &filters) {
    m_model->get(resourcePath, filters);
}

void SoundCloudPlaylistsWindow::showPlaylist(const QModelIndex &index) {
    if (SoundCloudPlaylist *playlist = m_model->get(index.row())) {
        SoundCloudPlaylistWindow *window = new SoundCloudPlaylistWindow(playlist, this);
        window->show();
    }
}

void SoundCloudPlaylistsWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void SoundCloudPlaylistsWindow::onModelStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
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
