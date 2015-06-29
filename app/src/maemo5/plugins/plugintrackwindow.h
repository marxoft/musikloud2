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

#ifndef PLUGINTRACKWINDOW_H
#define PLUGINTRACKWINDOW_H

#include "stackedwindow.h"
#include "plugincommentmodel.h"
#include "plugintrackmodel.h"

class PluginArtist;
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

class PluginTrackWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit PluginTrackWindow(const QString &service, const QString &id, StackedWindow *parent = 0);
    explicit PluginTrackWindow(PluginTrack *track, StackedWindow *parent = 0);
    ~PluginTrackWindow();
    
private:
    void loadBaseUi();
    void loadArtistUi();
    void loadTrackUi();
    
private Q_SLOTS:
    void getRelatedTracks();
    
    void downloadTrack();
    void playTrack();
    void queueTrack();
    void shareTrack();
    
    void downloadRelatedTrack();
    void playRelatedTrack(const QModelIndex &index);
    void queueRelatedTrack();
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
    void onCommentModelStatusChanged(ResourcesRequest::Status status);
    void onRelatedModelStatusChanged(ResourcesRequest::Status status);
    void onTabIndexChanged(int index);
    void onArtistStatusChanged(ResourcesRequest::Status status);
    void onTrackStatusChanged(ResourcesRequest::Status status);
        
private:
    PluginTrack *m_track;
    PluginArtist *m_artist;
    PluginTrackModel *m_relatedModel;
    PluginCommentModel *m_commentModel;
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
    QMenu *m_contextMenu;
    QAction *m_relatedQueueAction;
    QAction *m_relatedDownloadAction;
    QAction *m_relatedShareAction;
    QGridLayout *m_layout;
};
    
#endif // PLUGINTRACKWINDOW_H
