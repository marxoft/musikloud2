/*
 * Copyright (C) 2017 Stuart Howarth <showarth@marxoft.co.uk>
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

import QtQuick 1.1
import com.nokia.symbian 1.1
import MusiKloud 2.0
import QSoundCloud 1.0 as QSoundCloud
import ".."

MyPage {
    id: root
    
    function load(playlistOrId) {
        playlist.loadPlaylist(playlistOrId);

        if (Qt.isQtObject(playlistOrId)) {
            trackModel.get("/playlists/" + playlist.id);
        }
    }
    
    title: playlist.title ? playlist.title : qsTr("Set")
    tools: ToolBarLayout {
        BackToolButton {
            onClicked: {
                playlist.cancel();
                trackModel.cancel();
            }
        }
        
        NowPlayingButton {}
        
        MyToolButton {            
            iconSource: "toolbar-view-menu"
            toolTip: qsTr("Options")
            onClicked: popups.open(menu, root)
        }
    }
    
    SoundCloudPlaylist {
        id: playlist
        
        onStatusChanged: {
            switch (status) {
            case QSoundCloud.ResourcesRequest.Loading:
                root.showProgressIndicator = true;
                return;
            case QSoundCloud.ResourcesRequest.Ready:
                trackModel.get("/playlists/" + id);
                return;
            case QSoundCloud.ResourcesRequest.Failed:
                infoBanner.information(errorString);
                break;
            default:
                break;
            }

            root.showProgressIndicator = false;
        }
    }

    SoundCloudTrackModel {
        id: trackModel

        onStatusChanged: {
            switch (status) {
            case QSoundCloud.ResourcesRequest.Loading:
                root.showProgressIndicator = true;
                return;
            case QSoundCloud.ResourcesRequest.Ready:
                if (count == 0) {
                    infoBanner.information(qsTr("Set has no tracks"));
                }

                break;
            case QSoundCloud.ResourcesRequest.Failed:
                infoBanner.information(errorString);
                break;
            default:
                break;
            }

            root.showProgressIndicator = false;
        }
    }
    
    MyFlickable {
        id: flickable
        
        anchors.fill: parent
        contentHeight: flow.height + platformStyle.paddingLarge * 2
        
        Flow {
            id: flow
            
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: platformStyle.paddingLarge
            }
            spacing: platformStyle.paddingLarge
            
            Label {
                id: titleLabel
                
                width: parent.width
                wrapMode: Text.Wrap
                font.pixelSize: platformStyle.fontSizeLarge
                text: playlist.title ? playlist.title : qsTr("Unknown title")
            }

            HeaderLabel {
                id: infoHeader

                width: parent.width
                text: qsTr("Details")
            }
            
            Thumbnail {
                id: thumbnail
                
                width: column.height
                height: column.height
                source: playlist.thumbnailUrl
                placeholderSource: "../images/track.jpg"
                enabled: trackModel.count > 0
                onClicked: {
                    var tracks = [];
                    
                    for (var i = 0; i < trackModel.count; i++) {
                        tracks.push(trackModel.get(i));
                    }
                    
                    media.playTracks(tracks);
                }
            }
            
            Column {
                id: column
                
                width: parent.width - thumbnail.width - parent.spacing
                spacing: platformStyle.paddingLarge
                
                Label {
                    id: artistLabel

                    width: parent.width
                    elide: Text.ElideRight
                    clip: true
                    text: qsTr("Artist") + ": " + (playlist.artistId ? "<a href='" + playlist.artistId + "'>"
                                                   + playlist.artist + "</a>" : qsTr("Unknown"))
                    onLinkActivated: appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudArtistPage.qml"))
                    .load(playlist.artistId)
                }

                Label {
                    id: genreLabel

                    width: parent.width
                    elide: Text.ElideRight
                    text: qsTr("Genre") + ": " + (playlist.genre ? playlist.genre : qsTr("Unknown"))
                }

                Label {
                    id: durationLabel

                    width: parent.width
                    elide: Text.ElideRight
                    text: qsTr("Duration") + ": "
                    + (playlist.durationString ? playlist.durationString : qsTr("Unknown"))
                }
            }
            
            HeaderLabel {
                id: descriptionHeader

                width: parent.width
                text: qsTr("Description")
            }

            Label {
                id: descriptionLabel
                
                width: parent.width
                wrapMode: Text.Wrap
                text: playlist.description ? utils.toRichText(playlist.description) : qsTr("No description")
                onLinkActivated: {
                    var resource = mkresources.getResourceFromUrl(link);
                    
                    if (resource.service !== mkresources.SOUNDCLOUD) {
                        Qt.openUrlExternally(link);
                        return;
                    }
                    
                    switch (resource.type) {
                    case mkresources.ARTIST:
                        appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudArtistPage.qml")).load(resource.id);
                        break;
                    case mkresources.PLAYLIST:
                        appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudPlaylistPage.qml")).load(resource.id);
                        break;
                    case mkresources.TRACK:
                        appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudTrackPage.qml")).load(resource.id);
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
    
    ScrollDecorator {
        flickableItem: flickable
    }
    
    PopupLoader {
        id: popups
    }
    
    Component {
        id: menu
        
        MyMenu {
            focusItem: flickable
            
            MenuLayout {
                MenuItem {
                    text: qsTr("Play")
                    enabled: trackModel.count > 0
                    onClicked: {
                        var tracks = [];
                        
                        for (var i = 0; i < trackModel.count; i++) {
                            tracks.push(trackModel.get(i));
                        }
                        
                        media.playTracks(tracks);
                    }
                }
                
                MenuItem {
                    text: qsTr("Queue")
                    enabled: trackModel.count > 0
                    onClicked: {
                        var tracks = [];
                        
                        for (var i = 0; i < trackModel.count; i++) {
                            tracks.push(trackModel.get(i));
                        }
                        
                        media.addTracks(tracks);
                    }
                }

                MenuItem {
                    text: qsTr("Copy URL")
                    onClicked: {
                        clipboard.text = playlist.url;
                        infoBanner.information(qsTr("URL copied to clipboard"));
                    }
                }

                MenuItem {
                    text: qsTr("Tracks")
                    enabled: trackModel.count > 0
                    onClicked: appWindow.pageStack.push(tracksPage)
                }
            }
        }
    }

    Component {
        id: tracksPage

        MyPage {
            id: root

            title: qsTr("Tracks")
            tools: ToolBarLayout {
                BackToolButton {}

                NowPlayingButton {}

                MyToolButton {
                    iconSource: "toolbar-view-menu"
                    toolTip: qsTr("Options")
                    onClicked: popups.open(menu, root)
                }
            }

            MyListView {
                id: view

                anchors.fill: parent
                model: trackModel
                delegate: TrackDelegate {
                    onActivated: media.playTrack(trackModel.get(index))
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudTrackPage.qml"))
                    .load(trackModel.get(index))
                    onPressAndHold: popups.open(contextMenu, root)
                }
            }

            ScrollDecorator {
                flickableItem: view
            }

            Component {
                id: menu

                MyMenu {
                    focusItem: view

                    MenuLayout {
                        MenuItem {
                            text: qsTr("Play")
                            enabled: trackModel.count > 0
                            onClicked: {
                                var tracks = [];

                                for (var i = 0; i < trackModel.count; i++) {
                                    tracks.push(trackModel.get(i));
                                }

                                media.playTracks(tracks);
                            }
                        }

                        MenuItem {
                            text: qsTr("Queue")
                            enabled: trackModel.count > 0
                            onClicked: {
                                var tracks = [];

                                for (var i = 0; i < trackModel.count; i++) {
                                    tracks.push(trackModel.get(i));
                                }

                                media.addTracks(tracks);
                            }
                        }

                        MenuItem {
                            text: qsTr("Copy URL")
                            onClicked: {
                                clipboard.text = playlist.url;
                                infoBanner.information(qsTr("URL copied to clipboard"));
                            }
                        }
                    }
                }
            }

            Component {
                id: contextMenu

                MyContextMenu {
                    focusItem: view

                    MenuLayout {
                        MenuItem {
                            text: qsTr("Play")
                            onClicked: media.playTrack(trackModel.get(view.currentIndex))
                        }

                        MenuItem {
                            text: qsTr("Queue")
                            onClicked: media.addTrack(trackModel.get(view.currentIndex))
                        }

                        MenuItem {
                            text: qsTr("Download")
                            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudDownloadPage.qml"))
                            .get(trackModel.itemData(view.currentIndex));
                        }

                        MenuItem {
                            text: qsTr("Copy URL")
                            onClicked: {
                                clipboard.text = trackModel.data(view.currentIndex, "url");
                                infoBanner.information(qsTr("URL copied to clipboard"));
                            }
                        }

                        MenuItem {
                            text: trackModel.data(view.currentIndex, "favourited") ? qsTr("Unfavourite") : qsTr("Favourite")
                            enabled: soundcloud.userId !== ""
                            onClicked: {
                                var track = trackModel.get(view.currentIndex);

                                if (track.favourited) {
                                    track.unfavourite();
                                }
                                else {
                                    track.favourite();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
