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

#ifndef SOUNDCLOUDTRACKWINDOW_H
#define SOUNDCLOUDTRACKWINDOW_H

#include "stackedwindow.h"
#include "soundcloudcommentmodel.h"
#include "soundcloudtrackmodel.h"

class SoundCloudArtist;
class TrackDelegate;
class CommentDelegate;
class Image;
class ImageCache;
class NowPlayingAction;
class ListView;
class TextBrowser;
class QLabel;
class QScrollArea;
class QTabBar;
class QStackedWidget;
class QGridLayout;
class QModelIndex;

class SoundCloudTrackWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit SoundCloudTrackWindow(const QString &id, StackedWindow *parent = 0);
    explicit SoundCloudTrackWindow(SoundCloudTrack *track, StackedWindow *parent = 0);
    ~SoundCloudTrackWindow();
    
private:
    void loadBaseUi();
    void loadArtistUi();
    void loadTrackUi();
    
private Q_SLOTS:    
    void getRelatedTracks();
    
    void addComment();
    void downloadTrack();
    void playTrack();
    void queueTrack();
    void setTrackFavourite();
    void shareTrack();
    
    void downloadRelatedTrack();
    void playRelatedTrack(const QModelIndex &index);
    void queueRelatedTrack();
    void setRelatedTrackFavourite();
    void shareRelatedTrack();
    void showRelatedTrack(const QModelIndex &index);
    
    void reload();
    
    void showContextMenu(const QPoint &pos);
    
    void showArtist();
    void showArtist(const QModelIndex &index);
    void showComments();
    void showRelatedTracks();
    void showResource(const QUrl &url);
    
    void onImageReady();
    void onCommentModelStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    void onRelatedModelStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    void onTabIndexChanged(int index);
    void onArtistStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    void onTrackStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    void onTrackUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status status);
        
private:
    SoundCloudTrack *m_track;
    SoundCloudArtist *m_artist;
    SoundCloudTrackModel *m_relatedModel;
    SoundCloudCommentModel *m_commentModel;
    ImageCache *m_cache;
    
    Image *m_thumbnail;
    Image *m_avatar;
    NowPlayingAction *m_nowPlayingAction;
    ListView *m_relatedView;
    ListView *m_commentView;
    TrackDelegate *m_relatedDelegate;
    CommentDelegate *m_commentDelegate;
    QScrollArea *m_scrollArea;
    QTabBar *m_tabBar;
    QStackedWidget *m_stack;
    QLabel *m_titleLabel;
    TextBrowser *m_descriptionLabel;
    QLabel *m_dateLabel;
    QLabel *m_artistLabel;
    QLabel *m_noTracksLabel;
    QLabel *m_noCommentsLabel;
    QAction *m_reloadAction;
    QAction *m_queueAction;
    QAction *m_downloadAction;
    QAction *m_shareAction;
    QAction *m_favouriteAction;
    QAction *m_commentAction;
    QMenu *m_contextMenu;
    QAction *m_relatedQueueAction;
    QAction *m_relatedDownloadAction;
    QAction *m_relatedShareAction;
    QAction *m_relatedFavouriteAction;
    QGridLayout *m_layout;
};
    
#endif // SOUNDCLOUDTRACKWINDOW_H
