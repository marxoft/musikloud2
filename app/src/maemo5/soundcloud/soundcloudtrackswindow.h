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

#ifndef SOUNDCLOUDTRACKSWINDOW_H
#define SOUNDCLOUDTRACKSWINDOW_H

#include "stackedwindow.h"
#include "soundcloudtrackmodel.h"

class TrackDelegate;
class ImageCache;
class NowPlayingAction;
class ListView;
class QLabel;
class QVBoxLayout;

class SoundCloudTracksWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit SoundCloudTracksWindow(StackedWindow *parent = 0);
    ~SoundCloudTracksWindow();

public Q_SLOTS:
    void get(const QString &resourcePath, const QVariantMap &filters = QVariantMap());
    
private Q_SLOTS:    
    void downloadTrack(const QModelIndex &index);
    void playTrack(const QModelIndex &index);
    void queueTrack(const QModelIndex &index);
    void setTrackFavourite(const QModelIndex &index);
    void shareTrack(const QModelIndex &index);
    void showTrack(const QModelIndex &index);
    
    void showContextMenu(const QPoint &pos);
    
    void onImageReady();
    void onModelStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    void onTrackUpdateStatusChanged(QSoundCloud::ResourcesRequest::Status status);
    
private:
    SoundCloudTrackModel *m_model;
    ImageCache *m_cache;
    
    NowPlayingAction *m_nowPlayingAction;
    ListView *m_view;
    TrackDelegate *m_delegate;
    QAction *m_reloadAction;
    QLabel *m_label;
    QVBoxLayout *m_layout;
};
    
#endif // SOUNDCLOUDTRACKSWINDOW_H
