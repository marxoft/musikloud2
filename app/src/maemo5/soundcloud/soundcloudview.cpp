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

#include "soundcloudview.h"
#include "definitions.h"
#include "listview.h"
#include "navdelegate.h"
#include "resources.h"
#include "settings.h"
#include "soundcloud.h"
#include "soundcloudaccountswindow.h"
#include "soundcloudartist.h"
#include "soundcloudartistswindow.h"
#include "soundcloudartistwindow.h"
#include "soundcloudnavmodel.h"
#include "soundcloudplaylist.h"
#include "soundcloudplaylistswindow.h"
#include "soundcloudplaylistwindow.h"
#include "soundcloudsearchdialog.h"
#include "soundcloudtrack.h"
#include "soundcloudtrackswindow.h"
#include "soundcloudtrackwindow.h"
#include "stackedwindow.h"
#include <QVBoxLayout>
#include <QMaemo5InformationBox>

SoundCloudView::SoundCloudView(QWidget *parent) :
    QWidget(parent),
    m_model(new SoundCloudNavModel(this)),
    m_view(new ListView(this)),
    m_layout(new QVBoxLayout(this))
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    m_layout->addWidget(m_view);
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(onItemActivated(QModelIndex)));
    connect(SoundCloud::instance(), SIGNAL(commentAdded(SoundCloudComment*)),
            this, SLOT(onCommentAdded()));
    connect(SoundCloud::instance(), SIGNAL(artistFollowed(SoundCloudArtist*)),
            this, SLOT(onArtistFollowed(SoundCloudArtist*)));
    connect(SoundCloud::instance(), SIGNAL(artistUnfollowed(SoundCloudArtist*)),
            this, SLOT(onArtistUnfollowed(SoundCloudArtist*)));
    connect(SoundCloud::instance(), SIGNAL(trackFavourited(SoundCloudTrack*)),
            this, SLOT(onTrackFavourited(SoundCloudTrack*)));
    connect(SoundCloud::instance(), SIGNAL(trackUnfavourited(SoundCloudTrack*)),
            this, SLOT(onTrackUnfavourited(SoundCloudTrack*)));
}

void SoundCloudView::search(const QString &query, const QString &type, const QString &order) {
    Q_UNUSED(order);
    
    QVariantMap filters;
    filters["q"] = query;
    filters["limit"] = MAX_RESULTS;
    
    if (type == Resources::PLAYLIST) {
        SoundCloudPlaylistsWindow *window = new SoundCloudPlaylistsWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->get("/playlists", filters);
        window->show();
    }
    else if (type == Resources::ARTIST) {
        SoundCloudArtistsWindow *window = new SoundCloudArtistsWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->get("/users", filters);
        window->show();
    }
    else {
        SoundCloudTracksWindow *window = new SoundCloudTracksWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->get("/tracks", filters);
        window->show();
    }
}

void SoundCloudView::showResource(const QString &type, const QString &id) {
    if (type == Resources::PLAYLIST) {
        SoundCloudPlaylistWindow *window = new SoundCloudPlaylistWindow(id, StackedWindow::currentWindow());
        window->show();
    }
    else if (type == Resources::ARTIST) {
        SoundCloudArtistWindow *window = new SoundCloudArtistWindow(id, StackedWindow::currentWindow());
        window->show();
    }
    else {
        SoundCloudTrackWindow *window = new SoundCloudTrackWindow(id, StackedWindow::currentWindow());
        window->show();
    }
}

void SoundCloudView::showAccounts() {
    SoundCloudAccountsWindow *window = new SoundCloudAccountsWindow(StackedWindow::currentWindow());
    window->show();
}

void SoundCloudView::showFavourites() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    SoundCloudTracksWindow *window = new SoundCloudTracksWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Favourites"));
    window->get("/me/favorites", filters);
    window->show();
}

void SoundCloudView::showFollowings() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    SoundCloudArtistsWindow *window = new SoundCloudArtistsWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Followings"));
    window->get("/me/followings", filters);
    window->show();
}

void SoundCloudView::showPlaylists() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    SoundCloudPlaylistsWindow *window = new SoundCloudPlaylistsWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Sets"));
    window->get("/me/playlists", filters);
    window->show();
}

void SoundCloudView::showSearchDialog() {
    SoundCloudSearchDialog *dialog = new SoundCloudSearchDialog(StackedWindow::currentWindow());
    dialog->open();
}

void SoundCloudView::showTracks() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    SoundCloudTracksWindow *window = new SoundCloudTracksWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Tracks"));
    window->get("/me/tracks", filters);
    window->show();
}

void SoundCloudView::onItemActivated(const QModelIndex &index) {
    switch (index.row()) {
    case 0:
        showAccounts();
        break;
    case 1:
        showSearchDialog();
        break;
    case 2:
        showTracks();
        break;
    case 3:
        showFavourites();
        break;
    case 4:
        showPlaylists();
        break;
    case 5:
        showFollowings();
        break;
    default:
        break;
    }
}

void SoundCloudView::onArtistFollowed(SoundCloudArtist *artist) {
    QMaemo5InformationBox::information(this, tr("You have followed %1").arg(artist->name()));
}

void SoundCloudView::onArtistUnfollowed(SoundCloudArtist *artist) {
    QMaemo5InformationBox::information(this, tr("You have unfollowed %1").arg(artist->name()));
}

void SoundCloudView::onCommentAdded() {
    QMaemo5InformationBox::information(this, tr("Your comment has been added"));
}

void SoundCloudView::onTrackFavourited(SoundCloudTrack *track) {
    QMaemo5InformationBox::information(this, tr("'%1' added to favourites").arg(track->title()));
}

void SoundCloudView::onTrackUnfavourited(SoundCloudTrack *track) {
    QMaemo5InformationBox::information(this, tr("'%1' removed from favourites").arg(track->title()));
}
