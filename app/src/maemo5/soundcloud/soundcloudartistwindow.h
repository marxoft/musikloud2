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

#ifndef SOUNDCLOUDARTISTWINDOW_H
#define SOUNDCLOUDARTISTWINDOW_H

#include "stackedwindow.h"
#include "soundcloudartist.h"

class Image;
class NowPlayingAction;
class ListView;
class TextBrowser;
class QScrollArea;
class QLabel;
class QPushButton;
class QHBoxLayout;
class QModelIndex;
class QStringListModel;

class SoundCloudArtistWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit SoundCloudArtistWindow(const QString &id, StackedWindow *parent = 0);
    explicit SoundCloudArtistWindow(SoundCloudArtist *artist, StackedWindow *parent = 0);
    
private:
    void loadBaseUi();
    void loadArtistUi();
    
    void showFavourites();
    void showFollowings();
    void showPlaylists();
    void showTracks();
    
private Q_SLOTS:
    void setArtistFollowed();

    void showResource(const QUrl &url);    
    void showWindow(const QModelIndex &index);
    
    void onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    void onArtistUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    void onArtistFollowedChanged();

private:
    SoundCloudArtist *m_artist;
    QStringListModel *m_model;
    
    Image *m_avatar;
    NowPlayingAction *m_nowPlayingAction;
    ListView *m_view;
    QScrollArea *m_scrollArea;
    QLabel *m_titleLabel;
    QLabel *m_statsLabel;
    TextBrowser *m_descriptionLabel;
    QPushButton *m_followButton;
    QHBoxLayout *m_layout;
};

#endif // SOUNDCLOUDARTISTWINDOW_H
