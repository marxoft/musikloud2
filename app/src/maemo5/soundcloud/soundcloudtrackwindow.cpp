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

#include "soundcloudtrackwindow.h"
#include "audioplayer.h"
#include "clipboard.h"
#include "commentdelegate.h"
#include "definitions.h"
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
#include "soundcloudcommentdialog.h"
#include "soundcloudcommentmodel.h"
#include "soundclouddownloaddialog.h"
#include "soundcloudplaylistwindow.h"
#include "textbrowser.h"
#include "trackdelegate.h"
#include "utils.h"
#include <qsoundcloud/urls.h>
#include <QScrollArea>
#include <QTabBar>
#include <QStackedWidget>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QDesktopServices>
#include <QMaemo5InformationBox>

SoundCloudTrackWindow::SoundCloudTrackWindow(const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_track(new SoundCloudTrack(this)),
    m_artist(new SoundCloudArtist(this)),
    m_relatedModel(new SoundCloudTrackModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new Image(this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new TrackDelegate(m_cache, SoundCloudTrackModel::ArtistRole, SoundCloudTrackModel::DateRole,
                                        SoundCloudTrackModel::DurationStringRole, SoundCloudTrackModel::ThumbnailUrlRole,
                                        SoundCloudTrackModel::TitleRole, this)),
    m_commentDelegate(0),
    m_scrollArea(new QScrollArea(this)),
    m_tabBar(new QTabBar(this)),
    m_stack(new QStackedWidget(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_artistLabel(new QLabel(this)),
    m_noTracksLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No tracks found")), this)),
    m_noCommentsLabel(0),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_queueAction(new QAction(tr("Queue"), this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_favouriteAction(0),
    m_commentAction(0),
    m_contextMenu(new QMenu(this)),
    m_relatedQueueAction(new QAction(tr("Queue"), this)),
    m_relatedDownloadAction(new QAction(tr("Download"), this)),
    m_relatedShareAction(new QAction(tr("Copy URL"), this)),
    m_relatedFavouriteAction(0)
{
    loadBaseUi();
    connect(m_track, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
            this, SLOT(onTrackStatusChanged(QSoundCloud::ResourcesRequest::Status)));
            
    m_track->loadTrack(id);
}

SoundCloudTrackWindow::SoundCloudTrackWindow(SoundCloudTrack *track, StackedWindow *parent) :
    StackedWindow(parent),
    m_track(new SoundCloudTrack(track, this)),
    m_artist(new SoundCloudArtist(this)),
    m_relatedModel(new SoundCloudTrackModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new Image(this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new TrackDelegate(m_cache, SoundCloudTrackModel::ArtistRole, SoundCloudTrackModel::DateRole,
                                        SoundCloudTrackModel::DurationStringRole, SoundCloudTrackModel::ThumbnailUrlRole,
                                        SoundCloudTrackModel::TitleRole, this)),
    m_commentDelegate(0),
    m_scrollArea(new QScrollArea(this)),
    m_tabBar(new QTabBar(this)),
    m_stack(new QStackedWidget(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_artistLabel(new QLabel(this)),
    m_noTracksLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No tracks found")), this)),
    m_noCommentsLabel(0),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_queueAction(new QAction(tr("Queue"), this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_favouriteAction(0),
    m_commentAction(0),
    m_contextMenu(new QMenu(this)),
    m_relatedQueueAction(new QAction(tr("Queue"), this)),
    m_relatedDownloadAction(new QAction(tr("Download"), this)),
    m_relatedShareAction(new QAction(tr("Copy URL"), this)),
    m_relatedFavouriteAction(0)
{
    loadBaseUi();
    loadTrackUi();
    getRelatedTracks();
    connect(m_artist, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)), this,
            SLOT(onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status)));
            
    m_artist->loadArtist(track->artistId());
}

SoundCloudTrackWindow::~SoundCloudTrackWindow() {
    delete m_cache;
    m_cache = 0;
}

void SoundCloudTrackWindow::loadBaseUi() {
    setWindowTitle(tr("Track"));
    setCentralWidget(new QWidget);
    
    m_relatedView->setModel(m_relatedModel);
    m_relatedView->setItemDelegate(m_relatedDelegate);
    m_relatedView->setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_thumbnail->setFixedSize(320, 320);
    m_thumbnail->setFallbackSource(QUrl::fromLocalFile("/usr/share/icons/hicolor/295x295/hildon/mediaplayer_default_album.png"));
    m_avatar->setFixedSize(60, 60);
    m_avatar->setFallbackSource(QUrl::fromLocalFile("/usr/share/icons/hicolor/64x64/hildon/general_default_avatar.png"));
    
    m_titleLabel->setWordWrap(true);
    m_dateLabel->setStyleSheet(QString("color: %1; font-size: 13pt").arg(palette().color(QPalette::Mid).name()));
    m_artistLabel->setStyleSheet("font-size: 13pt");

    m_reloadAction->setEnabled(false);
    
    m_contextMenu->addAction(m_relatedQueueAction);
    m_contextMenu->addAction(m_relatedDownloadAction);
    m_contextMenu->addAction(m_relatedShareAction);
    
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
    
    m_tabBar->addTab(tr("Related"));
    m_tabBar->addTab(tr("Comments"));
    m_tabBar->setExpanding(false);
    m_tabBar->setDrawBase(false);
    m_tabBar->setStyleSheet("QTabBar::tab { height: 40px; }");
    
    m_stack->addWidget(m_relatedView);
    m_stack->addWidget(m_noTracksLabel);
    
    m_layout = new QGridLayout(centralWidget());
    m_layout->addWidget(m_scrollArea, 0, 0, 2, 1);
    m_layout->addWidget(m_tabBar, 0, 1);
    m_layout->addWidget(m_stack, 1, 1);
    m_layout->setRowStretch(1, 1);
    m_layout->setColumnStretch(1, 1);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    menuBar()->addAction(m_reloadAction);
    menuBar()->addAction(m_queueAction);
    menuBar()->addAction(m_downloadAction);
    menuBar()->addAction(m_shareAction);
    menuBar()->addAction(m_nowPlayingAction);
    
    connect(m_relatedModel, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)), this,
            SLOT(onRelatedModelStatusChanged(QSoundCloud::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_relatedView, SIGNAL(activated(QModelIndex)), this, SLOT(showRelatedTrack(QModelIndex)));
    connect(m_relatedView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_relatedView, SIGNAL(scrollingStarted()), m_nowPlayingAction, SLOT(hide()));
    connect(m_relatedView, SIGNAL(scrollingStopped()), m_nowPlayingAction, SLOT(poke()));
    connect(m_relatedDelegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playRelatedTrack(QModelIndex)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));
    connect(m_reloadAction, SIGNAL(triggered()), this, SLOT(reload()));
    connect(m_thumbnail, SIGNAL(clicked()), this, SLOT(playTrack()));
    connect(m_avatar, SIGNAL(clicked()), this, SLOT(showArtist()));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_queueAction, SIGNAL(triggered()), this, SLOT(queueTrack()));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadTrack()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareTrack()));
    connect(m_relatedQueueAction, SIGNAL(triggered()), this, SLOT(queueRelatedTrack()));
    connect(m_relatedDownloadAction, SIGNAL(triggered()), this, SLOT(downloadRelatedTrack()));
    connect(m_relatedShareAction, SIGNAL(triggered()), this, SLOT(shareRelatedTrack()));
    
    if (!SoundCloud::instance()->userId().isEmpty()) {
        m_favouriteAction = new QAction(tr("Favourite"), this);
        m_relatedFavouriteAction = new QAction(this);
        m_commentAction = new QAction(tr("Add comment"), this);
        menuBar()->insertAction(m_nowPlayingAction, m_favouriteAction);
        menuBar()->insertAction(m_nowPlayingAction, m_commentAction);
        m_contextMenu->addAction(m_relatedFavouriteAction);
        connect(m_favouriteAction, SIGNAL(triggered()), this, SLOT(setTrackFavourite()));
        connect(m_relatedFavouriteAction, SIGNAL(triggered()), this, SLOT(setRelatedTrackFavourite()));
        connect(m_commentAction, SIGNAL(triggered()), this, SLOT(addComment()));
    }
}

void SoundCloudTrackWindow::loadArtistUi() {
    m_artistLabel->setText(m_artistLabel->fontMetrics().elidedText(m_artist->name(), Qt::ElideRight, 250));
    m_avatar->setSource(m_artist->thumbnailUrl());
}

void SoundCloudTrackWindow::loadTrackUi() {
    setWindowTitle(m_track->title());
    
    m_titleLabel->setText(m_track->title());
    m_dateLabel->setText(tr("Published on %1").arg(m_track->date()));
    m_descriptionLabel->setHtml(Utils::toRichText(m_track->description()));
    m_thumbnail->setSource(m_track->largeThumbnailUrl());
    
    if (m_favouriteAction) {
        m_favouriteAction->setText(m_track->isFavourite() ? tr("Unfavourite") : tr("Favourite"));
    }
}

void SoundCloudTrackWindow::getRelatedTracks() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    m_relatedModel->get(QString("/users/%1/tracks").arg(m_track->artistId()), filters);
}

void SoundCloudTrackWindow::addComment() {
    if (isBusy()) {
        return;
    }
    
    SoundCloudCommentDialog *dialog = new SoundCloudCommentDialog(m_track->id(), this);
    dialog->open();
}

void SoundCloudTrackWindow::downloadTrack() {
    if (isBusy()) {
        return;
    }
    
    SoundCloudDownloadDialog *dialog = new SoundCloudDownloadDialog(m_track->id(), m_track->title(), this);
    dialog->open();
}

void SoundCloudTrackWindow::playTrack() {
    if (isBusy()) {
        return;
    }
    
    AudioPlayer::instance()->playTrack(m_track);
    NowPlayingWindow *window = new NowPlayingWindow(this);
    window->show();
}

void SoundCloudTrackWindow::queueTrack() {
    if (isBusy()) {
        return;
    }
    
    AudioPlayer::instance()->addTrack(m_track);
    QMaemo5InformationBox::information(this, tr("'%1' added to playback queue").arg(m_track->title()));
}

void SoundCloudTrackWindow::setTrackFavourite() {
    if (isBusy()) {
        return;
    }
    
    connect(m_track, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
                this, SLOT(onTrackUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status)));
    
    if (m_track->isFavourite()) {
        m_track->unfavourite();
    }
    else {
        m_track->favourite();
    }
}

void SoundCloudTrackWindow::shareTrack() {
    Clipboard::instance()->setText(m_track->url().toString());
    QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
}

void SoundCloudTrackWindow::downloadRelatedTrack() {
    if ((!isBusy()) && (m_relatedView->currentIndex().isValid())) {
        QString id = m_relatedView->currentIndex().data(SoundCloudTrackModel::IdRole).toString();
        QString title = m_relatedView->currentIndex().data(SoundCloudTrackModel::TitleRole).toString();
        
        SoundCloudDownloadDialog *dialog = new SoundCloudDownloadDialog(id, title, this);
        dialog->open();
    }
}

void SoundCloudTrackWindow::playRelatedTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_relatedModel->get(index.row())) {
        AudioPlayer::instance()->playTrack(track);
        NowPlayingWindow *window = new NowPlayingWindow(this);
        window->show();
    }
}

void SoundCloudTrackWindow::queueRelatedTrack() {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_relatedModel->get(m_relatedView->currentIndex().row())) {
        AudioPlayer::instance()->addTrack(track);
        QMaemo5InformationBox::information(this, tr("'%1' added to playback queue").arg(track->title()));
    }
}   

void SoundCloudTrackWindow::setRelatedTrackFavourite() {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_relatedModel->get(m_relatedView->currentIndex().row())) {
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

void SoundCloudTrackWindow::shareRelatedTrack() {
    if (const SoundCloudTrack *track = m_relatedModel->get(m_relatedView->currentIndex().row())) {
        Clipboard::instance()->setText(track->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void SoundCloudTrackWindow::showRelatedTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (SoundCloudTrack *track = m_relatedModel->get(index.row())) {
        SoundCloudTrackWindow *window = new SoundCloudTrackWindow(track, this);
        window->show();
    }
}

void SoundCloudTrackWindow::reload() {
    if (m_tabBar->currentIndex() == 1) {
        if (m_commentModel) {
            m_commentModel->reload();
        }
    }
    else {
        m_relatedModel->reload();
    }
}

void SoundCloudTrackWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_relatedView->currentIndex().isValid())) {
        if (m_relatedFavouriteAction) {
            m_relatedFavouriteAction->setText(m_relatedView->currentIndex().data(SoundCloudTrackModel::FavouriteRole).toBool()
                                              ? tr("Unfavourite") : tr("Favourite"));
        }
        
        m_contextMenu->popup(pos, m_relatedQueueAction);
    }
}

void SoundCloudTrackWindow::showArtist() {
    if (isBusy()) {
        return;
    }
    
    SoundCloudArtistWindow *window = new SoundCloudArtistWindow(m_artist, this);
    window->show();
}

void SoundCloudTrackWindow::showArtist(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    SoundCloudArtistWindow *window = new SoundCloudArtistWindow(index.data(SoundCloudCommentModel::ArtistIdRole).toString(),
                                                                this);
    window->show();
}

void SoundCloudTrackWindow::showComments() {
    if (!m_commentView) {
        m_commentView = new ListView(this);
        m_commentDelegate = new CommentDelegate(m_cache, SoundCloudCommentModel::ArtistRole,
                                                SoundCloudCommentModel::BodyRole,
                                                SoundCloudCommentModel::DateRole,
                                                SoundCloudCommentModel::ThumbnailUrlRole, this);
        m_commentModel = new SoundCloudCommentModel(this);
        m_commentView->setUniformItemSizes(false);
        m_commentView->setModel(m_commentModel);
        m_commentView->setItemDelegate(m_commentDelegate);
        m_noCommentsLabel = new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No comments found")), this);
        m_stack->addWidget(m_commentView);
        m_stack->addWidget(m_noCommentsLabel);
        
        connect(m_commentDelegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(showArtist(QModelIndex)));
        connect(m_commentModel, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
                this, SLOT(onCommentModelStatusChanged(QSoundCloud::ResourcesRequest::Status)));
                
        QVariantMap filters;
        filters["limit"] = MAX_RESULTS;
        m_commentModel->get(QString("/tracks/%1/comments").arg(m_track->id()), filters);
    }
    
    if ((m_commentModel->rowCount() == 0) && (m_commentModel->status() != QSoundCloud::ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noCommentsLabel);
    }
    else {
        m_stack->setCurrentWidget(m_commentView);
    }
}

void SoundCloudTrackWindow::showRelatedTracks() {
    if ((m_relatedModel->rowCount() == 0) && (m_relatedModel->status() != QSoundCloud::ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noTracksLabel);
    }
    else {
        m_stack->setCurrentWidget(m_relatedView);
    }
}

void SoundCloudTrackWindow::showResource(const QUrl &url) {
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

void SoundCloudTrackWindow::onImageReady() {
    if ((m_commentView) && (m_commentView->isVisible())) {
        m_commentView->viewport()->update(m_commentView->viewport()->rect());
    }
    else {
        m_relatedView->viewport()->update(m_relatedView->viewport()->rect());
    }
}

void SoundCloudTrackWindow::onCommentModelStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
    switch (status) {
    case QSoundCloud::ResourcesRequest::Loading:
        showProgressIndicator();
        showComments();
        m_reloadAction->setEnabled(false);
        return;
    case QSoundCloud::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_commentModel->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    m_reloadAction->setEnabled(true);
    
    if (m_commentView->isVisible()) {
        showComments();
    }
}

void SoundCloudTrackWindow::onRelatedModelStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
    switch (status) {
    case QSoundCloud::ResourcesRequest::Loading:
        showProgressIndicator();
        showRelatedTracks();
        m_reloadAction->setEnabled(false);
        return;
    case QSoundCloud::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_relatedModel->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    m_reloadAction->setEnabled(true);
    
    if (m_relatedView->isVisible()) {
        showRelatedTracks();
    }
}

void SoundCloudTrackWindow::onTabIndexChanged(int index) {
    if (index == 1) {
        showComments();
    }
    else {
        showRelatedTracks();
    }
}

void SoundCloudTrackWindow::onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
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

void SoundCloudTrackWindow::onTrackStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
    switch (status) {
    case QSoundCloud::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QSoundCloud::ResourcesRequest::Ready:
        loadTrackUi();
        getRelatedTracks();
        connect(m_artist, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)), this,
                SLOT(onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status)));
        m_artist->loadArtist(m_track->artistId());
        break;
    case QSoundCloud::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_track->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_track, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
               this, SLOT(onTrackStatusChanged(QSoundCloud::ResourcesRequest::Status)));
}

void SoundCloudTrackWindow::onTrackUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
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
    
    if (track == m_track) {
        loadTrackUi();
    }
    
    hideProgressIndicator();
    disconnect(track, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
               this, SLOT(onTrackUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status)));
}
