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

#ifndef PLUGINARTISTWINDOW_H
#define PLUGINARTISTWINDOW_H

#include "stackedwindow.h"
#include "pluginartist.h"

class Image;
class NowPlayingAction;
class ListView;
class TextBrowser;
class QScrollArea;
class QLabel;
class QHBoxLayout;
class QModelIndex;
class QStringListModel;

class PluginArtistWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit PluginArtistWindow(const QString &service, const QString &id, StackedWindow *parent = 0);
    explicit PluginArtistWindow(PluginArtist *artist, StackedWindow *parent = 0);
    
private:
    void loadBaseUi();
    void loadArtistUi();
    
    void showPlaylists();
    void showTracks();
    
private Q_SLOTS:
    void showResource(const QUrl &url);
    void showWindow(const QModelIndex &index);
    
    void onArtistStatusChanged(ResourcesRequest::Status status);

private:
    PluginArtist *m_artist;
    QStringListModel *m_model;
    
    Image *m_avatar;
    NowPlayingAction *m_nowPlayingAction;
    ListView *m_view;
    QScrollArea *m_scrollArea;
    QLabel *m_titleLabel;
    TextBrowser *m_descriptionLabel;
    QHBoxLayout *m_layout;
};

#endif // PLUGINARTISTWINDOW_H
