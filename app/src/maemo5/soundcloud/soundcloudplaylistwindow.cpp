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

#include "soundcloudplaylistwindow.h"
#include "audioplayer.h"
#include "clipboard.h"
#include "image.h"
#include "imagecache.h"
#include "listview.h"
#include "nowplayingaction.h"
#include "nowplayingwindow.h"
#include "resources.h"
#include "settings.h"
#include "soundcloud.h"
#include "soundcloudartist.h"
#include "soundcloudartistwindow.h"
#include "soundclouddownloaddialog.h"
#include "soundcloudplaylistwindow.h"
#include "soundcloudtrackmodel.h"
#include "soundcloudtrackwindow.h"
#include "textbrowser.h"
#include "trackdelegate.h"
#include "utils.h"
#include <qsoundcloud/urls.h>
#include <QScrollArea>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QDesktopServices>
#include <QMaemo5InformationBox>

SoundCloudPlaylistWindow::SoundCloudPlaylistWindow(const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_playlist(new SoundCloudPlaylist(this)),
    m_artist(new SoundCloudArtist(this)),
    m_model(new SoundCloudTrackModel(this)),
    m_cache(new ImageCache),
    m_thumbnail(new Image(this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_delegate(new TrackDelegate(m_cache, SoundCloudTrackModel::ArtistRole, SoundCloudTrackModel::DateRole,
                                 SoundCloudTrackModel::DurationStringRole, SoundCloudTrackModel::ThumbnailUrlRole,
                                 SoundCloudTrackModel::TitleRole, this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_artistLabel(new QLabel(this)),
    m_noTracksLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No tracks found")), this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_queuePlaylistAction(new QAction(tr("Queue"), this)),
    m_contextMenu(new QMenu(this)),
    m_queueAction(new QAction(tr("Queue"), this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_favouriteAction(0)
{
    loadBaseUi();
    connect(m_playlist, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
            this, SLOT(onPlaylistStatusChanged(QSoundCloud::ResourcesRequest::Status)));
            
    m_playlist->loadPlaylist(id);
}

SoundCloudPlaylistWindow::SoundCloudPlaylistWindow(SoundCloudPlaylist *playlist, StackedWindow *parent) :
    StackedWindow(parent),
    m_playlist(new SoundCloudPlaylist(playlist, this)),
    m_artist(new SoundCloudArtist(this)),
    m_model(new SoundCloudTrackModel(this)),
    m_cache(new ImageCache),
    m_thumbnail(new Image(this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_delegate(new TrackDelegate(m_cache, SoundCloudTrackModel::ArtistRole, SoundCloudTrackModel::DateRole,
                                 SoundCloudTrackModel::DurationStringRole, SoundCloudTrackModel::ThumbnailUrlRole,
                                 SoundCloudTrackModel::TitleRole, this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_artistLabel(new QLabel(this)),
    m_noTracksLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No tracks found")), this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_queuePlaylistAction(new QAction(tr("Queue"), this)),
    m_contextMenu(new QMenu(this)),
    m_queueAction(new QAction(tr("Queue"), this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_favouriteAction(0)
{
    loadBaseUi();
    loadPlaylistUi();
    getTracks();
    connect(m_artist, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)), this,
            SLOT(onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status)));
    
    m_artist->loadArtist(playlist->artistId());
}

SoundCloudPlaylistWindow::~SoundCloudPlaylistWindow() {
    delete m_cache;
    m_cache = 0;
}

void SoundCloudPlaylistWindow::loadBaseUi() {
    setWindowTitle(tr("Set"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_thumbnail->setFixedSize(320, 320);
    m_thumbnail->setFallbackSource(QUrl::fromLocalFile("/usr/share/icons/hicolor/295x295/hildon/mediaplayer_default_album.png"));
    m_avatar->setFixedSize(60, 60);
    m_avatar->setFallbackSource(QUrl::fromLocalFile("/usr/share/icons/hicolor/64x64/hildon/general_default_avatar.png"));
    
    m_titleLabel->setWordWrap(true);
    m_dateLabel->setStyleSheet(QString("color: %1; font-size: 13pt").arg(palette().color(QPalette::Mid).name()));
    m_artistLabel->setStyleSheet("font-size: 13pt");
    m_noTracksLabel->hide();

    m_reloadAction->setEnabled(false);
    
    m_contextMenu->addAction(m_queueAction);
    m_contextMenu->addAction(m_downloadAction);
    m_contextMenu->addAction(m_shareAction);
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QGridLayout *grid = new QGridLayout(scrollWidget);
    grid->addWidget(m_thumbnail, 0, 0, 1, 2, Qt::AlignLeft);
    grid->addWidget(m_titleLabel, 1, 0, 1, 2);
    grid->addWidget(m_avatar, 2, 0, 2, 1, Qt::AlignLeft);
    grid->addWidget(m_artistLabel, 2, 1, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(m_dateLabel, 3, 1, Qt::AlignLeft | Qt::AlignBottom);
    grid->addWidget(m_descriptionLabel, 4, 0, 1, 2);
    grid->setRowStretch(4, 1);
    grid->setColumnStretch(1, 1);
    grid->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFixedWidth(330);
    
    m_layout = new QHBoxLayout(centralWidget());
    m_layout->addWidget(m_scrollArea);
    m_layout->addWidget(m_view);
    m_layout->addWidget(m_noTracksLabel);
    m_layout->setStretch(1, 1);
    m_layout->setStretch(2, 1);
    m_layout->setContentsMargins(0, 0, 0, 0);

    menuBar()->addAction(m_reloadAction);
    menuBar()->addAction(m_queuePlaylistAction);
    menuBar()->addAction(m_nowPlayingAction);
    
    connect(m_model, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(QSoundCloud::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showTrack(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_thumbnail, SIGNAL(clicked()), this, SLOT(playPlaylist()));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playTrack(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
    connect(m_queuePlaylistAction, SIGNAL(triggered()), this, SLOT(queuePlaylist()));
    connect(m_avatar, SIGNAL(clicked()), this, SLOT(showArtist()));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_queueAction, SIGNAL(triggered()), this, SLOT(queueTrack()));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadTrack()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareTrack()));
    
    if (!SoundCloud::instance()->userId().isEmpty()) {
        m_favouriteAction = new QAction(this);
        m_contextMenu->addAction(m_favouriteAction);
        connect(m_favouriteAction, SIGNAL(triggered()), this, SLOT(setTrackFavourite()));
    }
}

void SoundCloudPlaylistWindow::loadPlaylistUi() {
    setWindowTitle(m_playlist->title());
    
    m_titleLabel->setText(m_playlist->title());
    m_dateLabel->setText(tr("Published on %1").arg(m_playlist->date()));
    m_descriptionLabel->setHtml(Utils::toRichText(m_playlist->description()));
    m_thumbnail->setSource(m_playlist->largeThumbnailUrl());
}

void SoundCloudPlaylistWindow::loadArtistUi() {
    m_artistLabel->setText(m_artistLabel->fontMetrics().elidedText(m_artist->name(), Qt::ElideRight, 250));
    m_avatar->setSource(m_artist->thumbnailUrl());
}

void SoundCloudPlaylistWindow::getTracks() {
    m_model->get("/playlists/" + m_playlist->id());
}

void SoundCloudPlaylistWindow::playPlaylist() {
    if ((isBusy()) || (m_model->rowCount() == 0)) {
        return;
    }
    
    QList<MKTrack*> tracks;
    
    for (int i = 0; i < m_model->rowCount(); i++) {
        if (SoundCloudTrack *track = m_model->get(i)) {
            tracks << track;
        }
    }
    
    if (!tracks.isEmpty()) {
        AudioPlayer::instance()->playTracks(tracks);
        NowPlayingWindow *window = new NowPlayingWindow(this);
        window->show();
    }
}

void SoundCloudPlaylistWindow::queuePlaylist() {
    if ((isBusy()) || (m_model->rowCount() == 0)) {
        return;
    }
    
    QList<MKTrack*> tracks;
    
    for (int i = 0; i < m_model->rowCount(); i++) {
        if (SoundCloudTrack *track = m_model->get(i)) {
            tracks << track;
        }
    }
    
    if (!tracks.isEmpty()) {
        AudioPlayer::instance()->addTracks(tracks);
        QMaemo5InformationBox::information(this, tr("'%1' added to playback queue").arg(m_playlist->title()));
    }
}

void SoundCloudPlaylistWindow::downloadTrack() {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        QString id = m_view->currentIndex().data(SoundCloudTrackModel::IdRole).toString();
        QString title = m_view->currentIndex().data(SoundCloudTrackModel::TitleRole).toString();
        
        SoundCloudDownloadDialog *dialog = new SoundCloudDownloadDialog(id, title, this);
        dialog->open();
    }
}

void SoundCloudPlaylistWindow::playTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_model->get(index.row())) {
        AudioPlayer::instance()->playTrack(track);
        NowPlayingWindow *window = new NowPlayingWindow(this);
        window->show();
    }
}

void SoundCloudPlaylistWindow::queueTrack() {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_model->get(m_view->currentIndex().row())) {
        AudioPlayer::instance()->addTrack(track);
        QMaemo5InformationBox::information(this, tr("'%1' added to playback queue").arg(track->title()));
    }
}

void SoundCloudPlaylistWindow::setTrackFavourite() {
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

void SoundCloudPlaylistWindow::shareTrack() {
    if (const SoundCloudTrack *track = m_model->get(m_view->currentIndex().row())) {
        Clipboard::instance()->setText(track->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void SoundCloudPlaylistWindow::showTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_model->get(index.row())) {
        SoundCloudTrackWindow *window = new SoundCloudTrackWindow(track, this);
        window->show();
    }
}

void SoundCloudPlaylistWindow::showArtist() {
    SoundCloudArtistWindow *window = new SoundCloudArtistWindow(m_artist, this);
    window->show();
}

void SoundCloudPlaylistWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        if (m_favouriteAction) {
            m_favouriteAction->setText(m_view->currentIndex().data(SoundCloudTrackModel::FavouriteRole).toBool()
                                              ? tr("Unfavourite") : tr("Favourite"));
        }
        
        m_contextMenu->popup(pos, m_queueAction);
    }
}

void SoundCloudPlaylistWindow::showResource(const QUrl &url) {
    QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != Resources::SOUNDCLOUD) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        SoundCloudPlaylistWindow *window = new SoundCloudPlaylistWindow(resource.value("id").toString(), this);
        window->show();
    }
    else if (resource.value("type") == Resources::ARTIST) {
        SoundCloudArtistWindow *window = new SoundCloudArtistWindow(resource.value("id").toString(), this);
        window->show();
    }
    else {
        SoundCloudTrackWindow *window = new SoundCloudTrackWindow(resource.value("id").toString(), this);
        window->show();
    }
}

void SoundCloudPlaylistWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void SoundCloudPlaylistWindow::onModelStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
    switch (status) {
    case QSoundCloud::ResourcesRequest::Loading:
        showProgressIndicator();
        m_noTracksLabel->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case QSoundCloud::ResourcesRequest::Ready:
        if (m_model->rowCount() > 0) {
            m_model->fetchMore();
        }
        
        break;
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
        m_noTracksLabel->show();
    }
}

void SoundCloudPlaylistWindow::onPlaylistStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
    switch (status) {
    case QSoundCloud::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QSoundCloud::ResourcesRequest::Ready:
        loadPlaylistUi();
        getTracks();
        connect(m_artist, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)), this,
                SLOT(onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status)));
        m_artist->loadArtist(m_playlist->artistId());
        break;
    case QSoundCloud::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_playlist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_playlist, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
               this, SLOT(onPlaylistStatusChanged(QSoundCloud::ResourcesRequest::Status)));
}

void SoundCloudPlaylistWindow::onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
    switch (status) {
    case QSoundCloud::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QSoundCloud::ResourcesRequest::Ready:
        loadArtistUi();
        break;
    case QSoundCloud::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_artist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_artist, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
               this, SLOT(onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status)));
}

void SoundCloudPlaylistWindow::onTrackUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
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
