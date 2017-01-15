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

#include "pluginartistwindow.h"
#include "image.h"
#include "listview.h"
#include "navdelegate.h"
#include "nowplayingaction.h"
#include "pluginplaylistswindow.h"
#include "pluginplaylistwindow.h"
#include "pluginartistswindow.h"
#include "plugintrackswindow.h"
#include "plugintrackwindow.h"
#include "resources.h"
#include "textbrowser.h"
#include "utils.h"
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QMenuBar>
#include <QGridLayout>
#include <QMessageBox>
#include <QMaemo5InformationBox>
#include <QStringListModel>
#include <QDesktopServices>

PluginArtistWindow::PluginArtistWindow(const QString &service, const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_artist(new PluginArtist(this)),
    m_model(new QStringListModel(QStringList() << tr("Tracks") << tr("Playlists"), this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this))
{
    loadBaseUi();
    connect(m_artist, SIGNAL(statusChanged(ResourcesRequest::Status)),
            this, SLOT(onArtistStatusChanged(ResourcesRequest::Status)));
            
    m_artist->loadArtist(service, id);
}

PluginArtistWindow::PluginArtistWindow(PluginArtist *artist, StackedWindow *parent) :
    StackedWindow(parent),
    m_artist(new PluginArtist(artist, this)),
    m_model(new QStringListModel(QStringList() << tr("Tracks") << tr("Playlists"), this)),
    m_avatar(new Image(this)),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this))
{
    loadBaseUi();
    loadArtistUi();
}

void PluginArtistWindow::loadBaseUi() {
    setWindowTitle(tr("Artist"));
    setCentralWidget(new QWidget);
        
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    
    m_avatar->setFixedSize(100, 100);
    m_avatar->setFallbackSource(QUrl::fromLocalFile("/usr/share/icons/hicolor/96x96/hildon/general_default_avatar.png"));
    
    m_titleLabel->setWordWrap(true);
        
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
    vbox->addWidget(m_avatar, Qt::AlignLeft);
    vbox->addWidget(m_titleLabel, Qt::AlignLeft | Qt::AlignTop);
    vbox->addWidget(m_descriptionLabel, Qt::AlignTop);
    vbox->setStretch(2, 1);
    vbox->setContentsMargins(0, 0, 0, 0);
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
}

void PluginArtistWindow::loadArtistUi() {
    setWindowTitle(m_artist->name());
    
    m_titleLabel->setText(m_artist->name());
    m_descriptionLabel->setHtml(Utils::toRichText(m_artist->description()));
    m_avatar->setSource(m_artist->largeThumbnailUrl());
}

void PluginArtistWindow::showPlaylists() {
    const QString playlistsId = m_artist->playlistsId();
    
    if (playlistsId.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("This artist does not have any playlists"));
        return;
    }
    
    PluginPlaylistsWindow *window = new PluginPlaylistsWindow(this);
    window->setWindowTitle(tr("%1's playlists").arg(m_artist->name()));
    window->list(m_artist->service(), playlistsId);
    window->show();
}

void PluginArtistWindow::showTracks() {
    const QString tracksId = m_artist->tracksId();
    
    if (tracksId.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("This artist does not have any tracks"));
        return;
    }
    
    PluginTracksWindow *window = new PluginTracksWindow(this);
    window->setWindowTitle(tr("%1's tracks").arg(m_artist->name()));
    window->list(m_artist->service(), tracksId);
    window->show();
}

void PluginArtistWindow::showResource(const QUrl &url) {
    const QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != m_artist->service()) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        PluginPlaylistWindow *window = new PluginPlaylistWindow(m_artist->service(), resource.value("id").toString(), this);
        window->show();
    }
    else if (resource.value("type") == Resources::ARTIST) {
        PluginArtistWindow *window = new PluginArtistWindow(m_artist->service(), resource.value("id").toString(), this);
        window->show();
    }
    else {
        PluginTrackWindow *window = new PluginTrackWindow(m_artist->service(), resource.value("id").toString(), this);
        window->show();
    }
}

void PluginArtistWindow::showWindow(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    switch (index.row()) {
    case 0:
        showTracks();
        break;
    case 1:
        showPlaylists();
        break;
    default:
        break;
    }
}

void PluginArtistWindow::onArtistStatusChanged(ResourcesRequest::Status status) {
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
