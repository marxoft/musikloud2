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

#ifndef SOUNDCLOUDVIEW_H
#define SOUNDCLOUDVIEW_H

#include <QWidget>

class SoundCloudArtist;
class SoundCloudTrack;
class SoundCloudNavModel;
class ListView;
class QModelIndex;
class QVBoxLayout;

class SoundCloudView : public QWidget
{
    Q_OBJECT
    
public:
    explicit SoundCloudView(QWidget *parent = 0);
    
public Q_SLOTS:
    void search(const QString &query, const QString &type, const QString &order);
    void showResource(const QString &type, const QString &id);
    
private:
    void showAccounts();
    void showFavourites();
    void showFollowings();
    void showPlaylists();
    void showSearchDialog();
    void showTracks();
    
private Q_SLOTS:
    void onItemActivated(const QModelIndex &index);
        
    void onArtistFollowed(SoundCloudArtist *artist);
    void onArtistUnfollowed(SoundCloudArtist *artist);
    
    void onCommentAdded();
    
    void onTrackFavourited(SoundCloudTrack *track);
    void onTrackUnfavourited(SoundCloudTrack *track);
    
private:
    SoundCloudNavModel *m_model;
    
    ListView *m_view;
    QVBoxLayout *m_layout;
};

#endif // SOUNDCLOUDVIEW_H
