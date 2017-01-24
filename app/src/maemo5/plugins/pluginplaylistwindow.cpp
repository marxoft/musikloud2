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

#include "pluginplaylistwindow.h"
#include "audioplayer.h"
#include "clipboard.h"
#include "image.h"
#include "imagecache.h"
#include "listview.h"
#include "nowplayingaction.h"
#include "nowplayingwindow.h"
#include "pluginartist.h"
#include "pluginartistwindow.h"
#include "plugindownloaddialog.h"
#include "pluginplaylistwindow.h"
#include "plugintrackmodel.h"
#include "plugintrackwindow.h"
#include "resources.h"
#include "textbrowser.h"
#include "transfers.h"
#include "utils.h"
#include "trackdelegate.h"
#include <QScrollArea>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QDesktopServices>
#include <QMaemo5InformationBox>

PluginPlaylistWindow::PluginPlaylistWindow(const QString &service, const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_playlist(new PluginPlaylist(this)),
    m_artist(new PluginArtist(this)),
    m_model(new PluginTrackModel(this)),
    m_cache(new ImageCache),
    m_thumbnail(new Image(this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_delegate(new TrackDelegate(m_cache, PluginTrackModel::ArtistRole, PluginTrackModel::DateRole,
                                 PluginTrackModel::DurationStringRole, PluginTrackModel::ThumbnailUrlRole,
                                 PluginTrackModel::TitleRole, this)),
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
    m_shareAction(new QAction(tr("Copy URL"), this))
{
    loadBaseUi();
    connect(m_playlist, SIGNAL(statusChanged(ResourcesRequest::Status)),
            this, SLOT(onPlaylistStatusChanged(ResourcesRequest::Status)));
            
    m_playlist->loadPlaylist(service, id);
}

PluginPlaylistWindow::PluginPlaylistWindow(PluginPlaylist *playlist, StackedWindow *parent) :
    StackedWindow(parent),
    m_playlist(new PluginPlaylist(playlist, this)),
    m_artist(new PluginArtist(this)),
    m_model(new PluginTrackModel(this)),
    m_cache(new ImageCache),
    m_thumbnail(new Image(this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_delegate(new TrackDelegate(m_cache, PluginTrackModel::ArtistRole, PluginTrackModel::DateRole,
                                 PluginTrackModel::DurationStringRole, PluginTrackModel::ThumbnailUrlRole,
                                 PluginTrackModel::TitleRole, this)),
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
    m_shareAction(new QAction(tr("Copy URL"), this))
{
    loadBaseUi();
    loadPlaylistUi();
    getTracks();
    
    if (!playlist->artistId().isEmpty()) {
        connect(m_artist, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
                SLOT(onArtistStatusChanged(ResourcesRequest::Status)));
            
        m_artist->loadArtist(playlist->service(), playlist->artistId());
    }
}

PluginPlaylistWindow::~PluginPlaylistWindow() {
    delete m_cache;
    m_cache = 0;
}

void PluginPlaylistWindow::loadBaseUi() {
    setWindowTitle(tr("Playlist"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_thumbnail->setFixedSize(320, 320);
    m_thumbnail->setFallbackSource(QUrl::fromLocalFile("/usr/share/icons/hicolor/295x295/hildon/mediaplayer_default_album.png"));
    m_avatar->setFixedSize(60, 60);
    m_avatar->setFallbackSource(QUrl::fromLocalFile("/usr/share/icons/hicolor/64x64/hildon/general_default_avatar.png"));
    m_avatar->hide();
    
    m_titleLabel->setWordWrap(true);
    m_dateLabel->setStyleSheet(QString("color: %1; font-size: 13pt").arg(palette().color(QPalette::Mid).name()));
    m_dateLabel->hide();
    m_artistLabel->setStyleSheet("font-size: 13pt");
    m_artistLabel->hide();
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
    
    connect(m_model, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showTrack(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_thumbnail, SIGNAL(clicked()), this, SLOT(playPlaylist()));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playTrack(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
    connect(m_queuePlaylistAction, SIGNAL(triggered()), this, SLOT(queuePlaylist()));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_queueAction, SIGNAL(triggered()), this, SLOT(queueTrack()));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadTrack()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareTrack()));
}

void PluginPlaylistWindow::loadPlaylistUi() {
    setWindowTitle(m_playlist->title());
    
    m_titleLabel->setText(m_playlist->title());
    m_descriptionLabel->setHtml(Utils::toRichText(m_playlist->description()));
    m_thumbnail->setSource(m_playlist->largeThumbnailUrl());
    
    const QString date = m_playlist->date();
    
    if (!date.isEmpty()) {
        m_dateLabel->setText(tr("Published on %1").arg(date));
        m_dateLabel->show();
    }
}

void PluginPlaylistWindow::loadArtistUi() {
    m_artistLabel->setText(m_artistLabel->fontMetrics().elidedText(m_artist->name(), Qt::ElideRight, 250));
    m_avatar->setSource(m_artist->thumbnailUrl());
    m_artistLabel->show();
    m_avatar->show();
    connect(m_avatar, SIGNAL(clicked()), this, SLOT(showArtist()));
}

void PluginPlaylistWindow::getTracks() {
    const QString id = m_playlist->tracksId();
    
    if (!id.isEmpty()) {
        m_model->setService(m_playlist->service());
        m_model->list(id);
        m_noTracksLabel->hide();
        m_view->show();
    }
    else {
        m_view->hide();
        m_noTracksLabel->show();
    }
}

void PluginPlaylistWindow::playPlaylist() {
    if ((isBusy()) || (m_model->rowCount() == 0)) {
        return;
    }
    
    QList<MKTrack*> tracks;
    
    for (int i = 0; i < m_model->rowCount(); i++) {
        if (PluginTrack *track = m_model->get(i)) {
            tracks << track;
        }
    }
    
    if (!tracks.isEmpty()) {
        AudioPlayer::instance()->playTracks(tracks);
        NowPlayingWindow *window = new NowPlayingWindow(this);
        window->show();
    }
}

void PluginPlaylistWindow::queuePlaylist() {
    if ((isBusy()) || (m_model->rowCount() == 0)) {
        return;
    }
    
    QList<MKTrack*> tracks;
    
    for (int i = 0; i < m_model->rowCount(); i++) {
        if (PluginTrack *track = m_model->get(i)) {
            tracks << track;
        }
    }
    
    if (!tracks.isEmpty()) {
        AudioPlayer::instance()->addTracks(tracks);
        QMaemo5InformationBox::information(this, tr("'%1' added to playback queue").arg(m_playlist->title()));
    }
}

void PluginPlaylistWindow::downloadTrack() {
    if (isBusy()) {
        return;
    }
    
    const QModelIndex index = m_view->currentIndex();
    
    if (index.isValid()) {
        const QString id = index.data(PluginTrackModel::IdRole).toString();
        const QString title = index.data(PluginTrackModel::TitleRole).toString();
        const QUrl streamUrl = index.data(PluginTrackModel::StreamUrlRole).toString();
        
        PluginDownloadDialog dialog(m_model->service(), this);
        dialog.list(id, streamUrl.isEmpty());
        
        if (dialog.exec() == QDialog::Accepted) {
            Transfers::instance()->addDownloadTransfer(m_model->service(), id, dialog.streamId(), streamUrl, title,
                                                       dialog.category(), dialog.customCommand(),
                                                       dialog.customCommandOverrideEnabled());
        }
    }
}

void PluginPlaylistWindow::playTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (PluginTrack *track = m_model->get(index.row())) {
        AudioPlayer::instance()->playTrack(track);
        NowPlayingWindow *window = new NowPlayingWindow(this);
        window->show();
    }
}

void PluginPlaylistWindow::queueTrack() {
    if (isBusy()) {
        return;
    }
    
    if (PluginTrack *track = m_model->get(m_view->currentIndex().row())) {
        AudioPlayer::instance()->addTrack(track);
        QMaemo5InformationBox::information(this, tr("'%1' added to playback queue").arg(track->title()));
    }
}

void PluginPlaylistWindow::shareTrack() {
    if (const PluginTrack *track = m_model->get(m_view->currentIndex().row())) {
        Clipboard::instance()->setText(track->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void PluginPlaylistWindow::showTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (PluginTrack *track = m_model->get(index.row())) {
        PluginTrackWindow *window = new PluginTrackWindow(track, this);
        window->show();
    }
}

void PluginPlaylistWindow::showArtist() {
    if (isBusy()) {
        return;
    }
    
    PluginArtistWindow *window = new PluginArtistWindow(m_artist, this);
    window->show();
}

void PluginPlaylistWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        m_downloadAction->setEnabled(m_model->data(m_view->currentIndex(), PluginTrackModel::DownloadableRole).toBool());
        m_contextMenu->popup(pos, m_queueAction);
    }
}

void PluginPlaylistWindow::showResource(const QUrl &url) {
    const QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != m_playlist->service()) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        PluginPlaylistWindow *window = new PluginPlaylistWindow(m_playlist->service(), resource.value("id").toString(),
                                                                this);
        window->show();
    }
    else if (resource.value("type") == Resources::ARTIST) {
        PluginArtistWindow *window = new PluginArtistWindow(m_playlist->service(), resource.value("id").toString(), this);
        window->show();
    }
    else {
        PluginTrackWindow *window = new PluginTrackWindow(m_playlist->service(), resource.value("id").toString(), this);
        window->show();
    }
}

void PluginPlaylistWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void PluginPlaylistWindow::onModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        m_noTracksLabel->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case ResourcesRequest::Ready:
        if (m_model->rowCount() > 0) {
            m_model->fetchMore();
        }
        
        break;
    case ResourcesRequest::Failed:
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

void PluginPlaylistWindow::onPlaylistStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case ResourcesRequest::Ready:
        loadPlaylistUi();
        getTracks();
        
        if (!m_playlist->artistId().isEmpty()) {
            connect(m_artist, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
                    SLOT(onArtistStatusChanged(ResourcesRequest::Status)));
            m_artist->loadArtist(m_playlist->service(), m_playlist->artistId());
        }
        
        break;
    case ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_playlist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_playlist, SIGNAL(statusChanged(ResourcesRequest::Status)),
               this, SLOT(onPlaylistStatusChanged(ResourcesRequest::Status)));
}

void PluginPlaylistWindow::onArtistStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case ResourcesRequest::Ready:
        loadArtistUi();
        break;
    case ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_artist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_artist, SIGNAL(statusChanged(ResourcesRequest::Status)),
               this, SLOT(onArtistStatusChanged(ResourcesRequest::Status)));
}
