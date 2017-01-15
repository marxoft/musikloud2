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

#include "soundcloudartistwindow.h"
#include "definitions.h"
#include "image.h"
#include "listview.h"
#include "navdelegate.h"
#include "nowplayingaction.h"
#include "resources.h"
#include "soundcloud.h"
#include "soundcloudartistswindow.h"
#include "soundcloudplaylistswindow.h"
#include "soundcloudplaylistwindow.h"
#include "soundcloudtrackswindow.h"
#include "soundcloudtrackwindow.h"
#include "textbrowser.h"
#include "utils.h"
#include <qsoundcloud/urls.h>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QMenuBar>
#include <QGridLayout>
#include <QMessageBox>
#include <QStringListModel>
#include <QDesktopServices>

SoundCloudArtistWindow::SoundCloudArtistWindow(const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_artist(new SoundCloudArtist(this)),
    m_model(new QStringListModel(QStringList() << tr("Tracks") << tr("Favourites") << tr("Sets") << tr("Followings"),
                                 this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_statsLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_followButton(new QPushButton(tr("Follow"), this))
{
    loadBaseUi();
    connect(m_artist, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
            this, SLOT(onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status)));
            
    m_artist->loadArtist(id);
}

SoundCloudArtistWindow::SoundCloudArtistWindow(SoundCloudArtist *artist, StackedWindow *parent) :
    StackedWindow(parent),
    m_artist(new SoundCloudArtist(artist, this)),
    m_model(new QStringListModel(QStringList() << tr("Tracks") << tr("Favourites") << tr("Sets") << tr("Followings"),
                                 this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_statsLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_followButton(new QPushButton(this))
{
    loadBaseUi();
    loadArtistUi();
    connect(m_artist, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
            this, SLOT(onArtistUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status)));
    connect(m_artist, SIGNAL(followedChanged()), this, SLOT(onArtistFollowedChanged())); 
    
    if ((!m_artist->isFollowed()) && (!SoundCloud::userId().isEmpty())) {
        m_artist->checkIfFollowed();
    }
}

void SoundCloudArtistWindow::loadBaseUi() {
    setWindowTitle(tr("Artist"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    
    m_avatar->setFixedSize(100, 100);
    m_avatar->setFallbackSource(QUrl::fromLocalFile("/usr/share/icons/hicolor/96x96/hildon/general_default_avatar.png"));
    
    m_statsLabel->setStyleSheet("font-size: 13pt");
    
    m_followButton->setEnabled(false);
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QGridLayout *grid = new QGridLayout(scrollWidget);
    grid->addWidget(m_avatar, 0, 0, Qt::AlignLeft);
    grid->addWidget(m_titleLabel, 1, 0, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(m_statsLabel, 1, 1, Qt::AlignRight | Qt::AlignTop);
    grid->addWidget(m_followButton, 2, 0, Qt::AlignLeft);
    grid->addWidget(m_descriptionLabel, 3, 0, 1, 2, Qt::AlignTop);
    grid->setRowStretch(3, 1);
    grid->setColumnStretch(0, 1);
    grid->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFixedWidth(410);
    
    m_layout = new QHBoxLayout(centralWidget());
    m_layout->addWidget(m_scrollArea);
    m_layout->addWidget(m_view);
    m_layout->setStretch(1, 1);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    menuBar()->addAction(m_nowPlayingAction);
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showWindow(QModelIndex)));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_followButton, SIGNAL(clicked()), this, SLOT(setArtistFollowed()));
}

void SoundCloudArtistWindow::loadArtistUi() {
    setWindowTitle(m_artist->name());
    
    m_followButton->setEnabled((!SoundCloud::userId().isEmpty())
                               && (m_artist->id() != SoundCloud::userId()));
    m_followButton->setText(m_artist->isFollowed() ? tr("Unfollow") : tr("Follow"));
    m_titleLabel->setText(m_titleLabel->fontMetrics().elidedText(m_artist->name(), Qt::ElideRight, 250));
    m_statsLabel->setText(tr("%1 %2\n%3 %4").arg(Utils::formatLargeNumber(m_artist->followersCount()))
                                            .arg(tr("followers"))
                                            .arg(Utils::formatLargeNumber(m_artist->trackCount()))
                                            .arg(tr("tracks")));
    m_descriptionLabel->setHtml(Utils::toRichText(m_artist->description()));
    m_avatar->setSource(m_artist->thumbnailUrl());
}

void SoundCloudArtistWindow::showFavourites() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    SoundCloudTracksWindow *window = new SoundCloudTracksWindow(this);
    window->setWindowTitle(tr("%1's favourites").arg(m_artist->name()));
    window->get(QString("/users/%1/favorites").arg(m_artist->id()), filters);
    window->show();
}

void SoundCloudArtistWindow::showFollowings() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    SoundCloudArtistsWindow *window = new SoundCloudArtistsWindow(this);
    window->setWindowTitle(tr("%1's followings").arg(m_artist->name()));
    window->get(QString("/users/%1/followings").arg(m_artist->id()), filters);
    window->show();
}

void SoundCloudArtistWindow::showPlaylists() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    SoundCloudPlaylistsWindow *window = new SoundCloudPlaylistsWindow(this);
    window->setWindowTitle(tr("%1's sets").arg(m_artist->name()));
    window->get(QString("/users/%1/playlists").arg(m_artist->id()), filters);
    window->show();
}

void SoundCloudArtistWindow::showTracks() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    SoundCloudTracksWindow *window = new SoundCloudTracksWindow(this);
    window->setWindowTitle(tr("%1's tracks").arg(m_artist->name()));
    window->get(QString("/users/%1/tracks").arg(m_artist->id()), filters);
    window->show();
}

void SoundCloudArtistWindow::showResource(const QUrl &url) {
    const QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
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

void SoundCloudArtistWindow::showWindow(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    switch (index.row()) {
    case 0:
        showTracks();
        break;
    case 1:
        showFavourites();
        break;
    case 2:
        showPlaylists();
        break;
    case 3:
        showFollowings();
        break;
    default:
        break;
    }
}

void SoundCloudArtistWindow::setArtistFollowed() {
    if (isBusy()) {
        return;
    }
    
    if (m_artist->isFollowed()) {
        m_artist->unfollow();
    }
    else {
        m_artist->follow();
    }
}

void SoundCloudArtistWindow::onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
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
    connect(m_artist, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
            this, SLOT(onArtistUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status)));
    connect(m_artist, SIGNAL(followedChanged()), this, SLOT(onArtistFollowedChanged())); 
    
    if ((!m_artist->isFollowed()) && (!SoundCloud::userId().isEmpty())) {
        m_artist->checkIfFollowed();
    }
}

void SoundCloudArtistWindow::onArtistUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
    switch (status) {
    case QSoundCloud::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QSoundCloud::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_artist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
}

void SoundCloudArtistWindow::onArtistFollowedChanged() {
    m_followButton->setText(m_artist->isFollowed() ? tr("Unfollow") : tr("Follow"));
}
