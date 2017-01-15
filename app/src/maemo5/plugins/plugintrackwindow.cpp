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

#include "plugintrackwindow.h"
#include "audioplayer.h"
#include "clipboard.h"
#include "commentdelegate.h"
#include "image.h"
#include "imagecache.h"
#include "listview.h"
#include "nowplayingaction.h"
#include "nowplayingwindow.h"
#include "pluginartist.h"
#include "pluginartistwindow.h"
#include "plugincommentmodel.h"
#include "plugindownloaddialog.h"
#include "pluginplaylistwindow.h"
#include "resources.h"
#include "textbrowser.h"
#include "trackdelegate.h"
#include "transfers.h"
#include "utils.h"
#include <QScrollArea>
#include <QTabBar>
#include <QStackedWidget>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QDesktopServices>
#include <QMaemo5InformationBox>

PluginTrackWindow::PluginTrackWindow(const QString &service, const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_track(new PluginTrack(this)),
    m_artist(new PluginArtist(this)),
    m_relatedModel(new PluginTrackModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new Image(this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new TrackDelegate(m_cache, PluginTrackModel::ArtistRole, PluginTrackModel::DateRole,
                                        PluginTrackModel::DurationStringRole, PluginTrackModel::ThumbnailUrlRole,
                                        PluginTrackModel::TitleRole, this)),
    m_commentDelegate(0),
    m_scrollArea(new QScrollArea(this)),
    m_tabBar(new QTabBar(this)),
    m_stack(new QStackedWidget(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_artistLabel(new QLabel(this)),
    m_noResultsLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                .arg(palette().color(QPalette::Mid).name()).arg(tr("No results")), this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_queueAction(new QAction(tr("Queue"), this)),
    m_downloadAction(0),
    m_shareAction(new QAction(tr("Copy URL"), this))
{
    loadBaseUi();
    connect(m_track, SIGNAL(statusChanged(ResourcesRequest::Status)),
            this, SLOT(onTrackStatusChanged(ResourcesRequest::Status)));
    
    m_track->loadTrack(service, id);
}

PluginTrackWindow::PluginTrackWindow(PluginTrack *track, StackedWindow *parent) :
    StackedWindow(parent),
    m_track(new PluginTrack(track, this)),
    m_artist(new PluginArtist(this)),
    m_relatedModel(new PluginTrackModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new Image(this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new TrackDelegate(m_cache, PluginTrackModel::ArtistRole, PluginTrackModel::DateRole,
                                        PluginTrackModel::DurationStringRole, PluginTrackModel::ThumbnailUrlRole,
                                        PluginTrackModel::TitleRole, this)),
    m_commentDelegate(0),
    m_scrollArea(new QScrollArea(this)),
    m_tabBar(new QTabBar(this)),
    m_stack(new QStackedWidget(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_artistLabel(new QLabel(this)),
    m_noResultsLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                .arg(palette().color(QPalette::Mid).name()).arg(tr("No results")), this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_queueAction(new QAction(tr("Queue"), this)),
    m_downloadAction(0),
    m_shareAction(new QAction(tr("Copy URL"), this))
{
    loadBaseUi();
    loadTrackUi();
    getRelatedTracks();
    
    if (!track->artistId().isEmpty()) {
        connect(m_artist, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
                SLOT(onArtistStatusChanged(ResourcesRequest::Status)));
        
        m_artist->loadArtist(track->service(), track->artistId());
    }
}

PluginTrackWindow::~PluginTrackWindow() {
    delete m_cache;
    m_cache = 0;
}

void PluginTrackWindow::loadBaseUi() {
    setWindowTitle(tr("Track"));
    setCentralWidget(new QWidget);
    
    m_relatedView->setModel(m_relatedModel);
    m_relatedView->setItemDelegate(m_relatedDelegate);
    m_relatedView->setContextMenuPolicy(Qt::CustomContextMenu);
    
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

    m_reloadAction->setEnabled(false);
    
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
    m_tabBar->setExpanding(false);
    m_tabBar->setDrawBase(false);
    m_tabBar->setStyleSheet("QTabBar::tab { height: 40px; }");
    
    m_stack->addWidget(m_relatedView);
    m_stack->addWidget(m_noResultsLabel);
    
    m_layout = new QGridLayout(centralWidget());
    m_layout->addWidget(m_scrollArea, 0, 0, 2, 1);
    m_layout->addWidget(m_tabBar, 0, 1);
    m_layout->addWidget(m_stack, 1, 1);
    m_layout->setRowStretch(1, 1);
    m_layout->setColumnStretch(1, 1);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    menuBar()->addAction(m_reloadAction);
    menuBar()->addAction(m_queueAction);
    menuBar()->addAction(m_shareAction);
    menuBar()->addAction(m_nowPlayingAction);
    
    connect(m_relatedModel, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
            SLOT(onRelatedModelStatusChanged(ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_relatedView, SIGNAL(activated(QModelIndex)), this, SLOT(showRelatedTrack(QModelIndex)));
    connect(m_relatedView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_relatedDelegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playRelatedTrack(QModelIndex)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));
    connect(m_reloadAction, SIGNAL(triggered()), this, SLOT(reload()));
    connect(m_thumbnail, SIGNAL(clicked()), this, SLOT(playTrack()));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_queueAction, SIGNAL(triggered()), this, SLOT(queueTrack()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareTrack()));
    
    if (m_track->isDownloadable()) {
        m_downloadAction = new QAction(tr("Download"), this);
        menuBar()->insertAction(m_shareAction, m_downloadAction);
        connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadTrack()));
    }
}

void PluginTrackWindow::loadArtistUi() {
    m_artistLabel->setText(m_artistLabel->fontMetrics().elidedText(m_artist->name(), Qt::ElideRight, 250));
    m_avatar->setSource(m_artist->thumbnailUrl());
    m_artistLabel->show();
    m_avatar->show();
    connect(m_avatar, SIGNAL(clicked()), this, SLOT(showArtist()));
}

void PluginTrackWindow::loadTrackUi() {
    setWindowTitle(m_track->title());
    
    m_titleLabel->setText(m_track->title());
    m_descriptionLabel->setHtml(Utils::toRichText(m_track->description()));
    m_thumbnail->setSource(m_track->largeThumbnailUrl());
    
    const QString date = m_track->date();
    
    if (!date.isEmpty()) {
        m_dateLabel->setText(tr("Published on %1").arg(date));
        m_dateLabel->show();
    }
    
    if (!m_track->commentsId().isEmpty()) {
        m_tabBar->addTab(tr("Comments"));
    }
}

void PluginTrackWindow::getRelatedTracks() {
    const QString id = m_track->relatedTracksId();
    
    if (!id.isEmpty()) {
        m_relatedModel->setService(m_track->service());
        m_relatedModel->list(id);
    }
    else {
        m_stack->setCurrentWidget(m_noResultsLabel);
    }
}

void PluginTrackWindow::downloadTrack() {
    if (isBusy()) {
        return;
    }
    
    PluginDownloadDialog dialog(m_track->service(), this);
    dialog.list(m_track->id(), m_track->streamUrl().isEmpty());

    if (dialog.exec() == QDialog::Accepted) {
        Transfers::instance()->addDownloadTransfer(m_track->service(), m_track->id(), dialog.streamId(),
                                                   m_track->streamUrl(), m_track->title(), dialog.category(),
                                                   dialog.customCommand(), dialog.customCommandOverrideEnabled());
    }
}

void PluginTrackWindow::playTrack() {
    if (isBusy()) {
        return;
    }
    
    AudioPlayer::instance()->playTrack(m_track);
    NowPlayingWindow *window = new NowPlayingWindow(this);
    window->show();
}

void PluginTrackWindow::queueTrack() {
    if (isBusy()) {
        return;
    }
    
    AudioPlayer::instance()->addTrack(m_track);
    QMaemo5InformationBox::information(this, tr("'%1' added to playback queue").arg(m_track->title()));
}

void PluginTrackWindow::shareTrack() {
    Clipboard::instance()->setText(m_track->url().toString());
    QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
}

void PluginTrackWindow::downloadRelatedTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
        
    if (index.isValid()) {
        const QString id = index.data(PluginTrackModel::IdRole).toString();
        const QString title = index.data(PluginTrackModel::TitleRole).toString();
        const QUrl streamUrl = index.data(PluginTrackModel::StreamUrlRole).toString();
        
        PluginDownloadDialog dialog(m_relatedModel->service(), this);
        dialog.list(id, streamUrl.isEmpty());
        
        if (dialog.exec() == QDialog::Accepted) {
            Transfers::instance()->addDownloadTransfer(m_relatedModel->service(), id, dialog.streamId(), streamUrl,
                                                       title, dialog.category(), dialog.customCommand(),
                                                       dialog.customCommandOverrideEnabled());
        }
    }
}

void PluginTrackWindow::playRelatedTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (PluginTrack *track = m_relatedModel->get(index.row())) {
        AudioPlayer::instance()->playTrack(track);
        NowPlayingWindow *window = new NowPlayingWindow(this);
        window->show();
    }
}

void PluginTrackWindow::queueRelatedTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (PluginTrack *track = m_relatedModel->get(index.row())) {
        AudioPlayer::instance()->addTrack(track);
        QMaemo5InformationBox::information(this, tr("'%1' added to playback queue").arg(track->title()));
    }
}

void PluginTrackWindow::shareRelatedTrack(const QModelIndex &index) {
    if (const PluginTrack *track = m_relatedModel->get(index.row())) {
        Clipboard::instance()->setText(track->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void PluginTrackWindow::showRelatedTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (PluginTrack *track = m_relatedModel->get(index.row())) {
        PluginTrackWindow *window = new PluginTrackWindow(track, this);
        window->show();
    }
}

void PluginTrackWindow::reload() {
    if (m_tabBar->currentIndex() == 1) {
        if (m_commentModel) {
            m_commentModel->reload();
        }
    }
    else {
        m_relatedModel->reload();
    }
}

void PluginTrackWindow::showContextMenu(const QPoint &pos) {
    if (isBusy()) {
        return;
    }
    
    const QModelIndex index = m_relatedView->currentIndex();
    
    if (!index.isValid()) {
        return;
    }
    
    if (index.data(PluginTrackModel::DownloadableRole).toBool()) {
        QMenu menu(this);
        QAction *queueAction = menu.addAction(tr("Queue"));
        QAction *downloadAction = menu.addAction(tr("Download"));
        QAction *shareAction = menu.addAction(tr("Copy URL"));
        QAction *action = menu.exec(pos);
        
        if (!action) {
            return;
        }
        
        if (action == queueAction) {
            queueRelatedTrack(index);
        }
        else if (action == downloadAction) {
            downloadRelatedTrack(index);
        }
        else if (action == shareAction) {
            shareRelatedTrack(index);
        }
    }
    else {
        QMenu menu(this);
        QAction *queueAction = menu.addAction(tr("Queue"));
        QAction *shareAction = menu.addAction(tr("Copy URL"));
        QAction *action = menu.exec(pos);
        
        if (!action) {
            return;
        }
        
        if (action == queueAction) {
            queueRelatedTrack(index);
        }
        else if (action == shareAction) {
            shareRelatedTrack(index);
        }
    }
}

void PluginTrackWindow::showArtist() {
    if (isBusy()) {
        return;
    }
    
    PluginArtistWindow *window = new PluginArtistWindow(m_artist, this);
    window->show();
}

void PluginTrackWindow::showArtist(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    const QString id = index.data(PluginCommentModel::ArtistIdRole).toString();
    
    if (!id.isEmpty()) {
        PluginArtistWindow *window = new PluginArtistWindow(m_track->service(), id, this);
        window->show();
    }
}

void PluginTrackWindow::showComments() {
    if (!m_commentView) {
        m_commentView = new ListView(this);
        m_commentDelegate = new CommentDelegate(m_cache, PluginCommentModel::ArtistRole, PluginCommentModel::BodyRole,
                                                PluginCommentModel::DateRole, PluginCommentModel::ThumbnailUrlRole, this);
        m_commentModel = new PluginCommentModel(this);
        m_commentModel->setService(m_track->service());
        m_commentView->setUniformItemSizes(false);
        m_commentView->setModel(m_commentModel);
        m_commentView->setItemDelegate(m_commentDelegate);
        m_stack->addWidget(m_commentView);
        
        connect(m_commentDelegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(showArtist(QModelIndex)));
        connect(m_commentModel, SIGNAL(statusChanged(ResourcesRequest::Status)),
                this, SLOT(onCommentModelStatusChanged(ResourcesRequest::Status)));
                
        m_commentModel->list(m_track->commentsId());
    }
    
    if ((m_commentModel->rowCount() == 0) && (m_commentModel->status() != ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noResultsLabel);
    }
    else {
        m_stack->setCurrentWidget(m_commentView);
    }
}

void PluginTrackWindow::showRelatedTracks() {
    if ((m_relatedModel->rowCount() == 0) && (m_relatedModel->status() != ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noResultsLabel);
    }
    else {
        m_stack->setCurrentWidget(m_relatedView);
    }
}

void PluginTrackWindow::showResource(const QUrl &url) {
    const QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != m_track->service()) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        PluginPlaylistWindow *window = new PluginPlaylistWindow(m_track->service(), resource.value("id").toString(), this);
        window->show();
    }
    else if (resource.value("type") == Resources::ARTIST) {
        PluginArtistWindow *window = new PluginArtistWindow(m_track->service(), resource.value("id").toString(), this);
        window->show();
    }
    else {
        PluginTrackWindow *window = new PluginTrackWindow(m_track->service(), resource.value("id").toString(), this);
        window->show();
    }
}

void PluginTrackWindow::onImageReady() {
    if ((m_commentView) && (m_commentView->isVisible())) {
        m_commentView->viewport()->update(m_commentView->viewport()->rect());
    }
    else {
        m_relatedView->viewport()->update(m_relatedView->viewport()->rect());
    }
}

void PluginTrackWindow::onCommentModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        showComments();
        m_reloadAction->setEnabled(false);
        return;
    case ResourcesRequest::Failed:
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

void PluginTrackWindow::onRelatedModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        showRelatedTracks();
        m_reloadAction->setEnabled(false);
        return;
    case ResourcesRequest::Failed:
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

void PluginTrackWindow::onTabIndexChanged(int index) {
    if (index == 1) {
        showComments();
    }
    else {
        showRelatedTracks();
    }
}

void PluginTrackWindow::onArtistStatusChanged(ResourcesRequest::Status status) {
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

void PluginTrackWindow::onTrackStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case ResourcesRequest::Ready:
        loadTrackUi();
        getRelatedTracks();
        
        if (!m_track->artistId().isEmpty()) {
            connect(m_artist, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
                    SLOT(onArtistStatusChanged(ResourcesRequest::Status)));
            m_artist->loadArtist(m_track->service(), m_track->artistId());
        }
        
        break;
    case ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_track->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_track, SIGNAL(statusChanged(ResourcesRequest::Status)),
               this, SLOT(onTrackStatusChanged(ResourcesRequest::Status)));
}
