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

#ifndef SOUNDCLOUDPLAYLISTWINDOW_H
#define SOUNDCLOUDPLAYLISTWINDOW_H

#include "stackedwindow.h"
#include "soundcloudplaylist.h"

class SoundCloudArtist;
class TrackDelegate;
class SoundCloudTrackModel;
class Image;
class ImageCache;
class NowPlayingAction;
class ListView;
class TextBrowser;
class QLabel;
class QScrollArea;
class QHBoxLayout;
class QModelIndex;

class SoundCloudPlaylistWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit SoundCloudPlaylistWindow(const QString &id, StackedWindow *parent = 0);
    explicit SoundCloudPlaylistWindow(SoundCloudPlaylist *playlist, StackedWindow *parent = 0);
    ~SoundCloudPlaylistWindow();
    
private:
    void loadBaseUi();
    void loadPlaylistUi();
    void loadArtistUi();
    
private Q_SLOTS:
    void getTracks();
    
    void playPlaylist();
    void queuePlaylist();
    
    void downloadTrack();
    void playTrack(const QModelIndex &index);
    void queueTrack();
    void setTrackFavourite();
    void shareTrack();
    void showTrack(const QModelIndex &index);
    
    void showArtist();
    
    void showContextMenu(const QPoint &pos);
    
    void showResource(const QUrl &url);
    
    void onImageReady();
    void onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    void onModelStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    void onPlaylistStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    void onTrackUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    
private:
    SoundCloudPlaylist *m_playlist;
    SoundCloudArtist *m_artist;
    SoundCloudTrackModel *m_model;
    ImageCache *m_cache;
    
    Image *m_thumbnail;
    Image *m_avatar;
    NowPlayingAction *m_nowPlayingAction;
    ListView *m_view;
    TrackDelegate *m_delegate;
    QScrollArea *m_scrollArea;
    QLabel *m_titleLabel;
    TextBrowser *m_descriptionLabel;
    QLabel *m_dateLabel;
    QLabel *m_artistLabel;
    QLabel *m_noTracksLabel;
    QAction *m_reloadAction;
    QAction *m_queuePlaylistAction;
    QMenu *m_contextMenu;
    QAction *m_queueAction;
    QAction *m_downloadAction;
    QAction *m_shareAction;
    QAction *m_favouriteAction;
    QHBoxLayout *m_layout;
};
    
#endif // SOUNDCLOUDPLAYLISTWINDOW_H
