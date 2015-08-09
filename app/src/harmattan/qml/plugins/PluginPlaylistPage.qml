/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import MusiKloud 2.0
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    function load(playlistOrId) {
        if (playlistOrId.hasOwnProperty("id")) {
            playlist.loadPlaylist(playlistOrId);
            tracksTab.model.list(playlist.id);
        }
        else {
            playlist.loadPlaylist(Settings.currentService, playlistOrId)
            tracksTab.model.list(playlistOrId);
        }
    }

    tools: view.currentTab.tools ? view.currentTab.tools : ownTools
    onToolsChanged: if (status == PageStatus.Active) appWindow.pageStack.toolBar.setTools(tools, "set");

    ToolBarLayout {
        id: ownTools

        visible: false

        BackToolIcon {}
        
        NowPlayingButton {}
        
        MyToolIcon {
            platformIconId: "toolbar-view-menu"
            onClicked: menu.open()
        }
    }
    
    Menu {
        id: menu
        
        MenuLayout {
        
            MenuItem {
                text: qsTr("Add to queue")
                enabled: tracksTab.model.count > 0
                onClicked: {
                    var tracks = [];

                    for (var i = 0; i < tracksTab.model.count; i ++) {
                        tracks.push(tracksTab.model.get(i));
                    }

                    player.addTracks(tracks);
                    infoBanner.showMessage(tracks.length + " " + qsTr("tracks added to queue"));
                }
            }
        }
    }

    PluginPlaylist {
        id: playlist

        onStatusChanged: if (status == QSoundCloud.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
    }

    TabView {
        id: view

        anchors.fill: parent
        enabled: playlist.id != ""

        Tab {
            id: infoTab

            width: view.width
            height: view.height
            
            PageHeader {
                id: header
                
                title: playlist.title
                showProgressIndicator: (playlist.status == ResourcesRequest.Loading)
                                       || (tracksTab.model.status == ResourcesRequest.Loading)
            }

            Row {
                id: row
                
                anchors {
                    left: parent.left
                    right: parent.right
                    top: header.bottom
                    margins: UI.PADDING_DOUBLE
                }
                spacing: UI.PADDING_DOUBLE

                Thumbnail {
                    id: thumbnail

                    z: 10
                    width: 150
                    height: 150
                    source: playlist.largeThumbnailUrl
                    placeholderText: playlist.title
                    enabled: tracksTab.model.count > 0
                    onClicked: {
                        var tracks = [];

                        for (var i = 0; i < tracksTab.model.count; i ++) {
                            tracks.push(tracksTab.model.get(i));
                        }

                        player.clearQueue();
                        player.addTracks(tracks);
                        appWindow.pageStack.push(Qt.resolvedUrl("../NowPlayingPage.qml"), {autoPlay: true});
                    }
                }
                
                Column {
                    id: column
                    
                    width: row.width - thumbnail.width - row.spacing
                    spacing: UI.PADDING_DOUBLE
                    
                    Label {
                        width: parent.width
                        clip: true
                        font.family: UI.FONT_FAMILY_LIGHT
                        font.pixelSize: UI.FONT_SMALL
                        text: qsTr("Artist") + ": "
                              + (playlist.artist ? Plugins.resourceTypeIsSupported(Settings.currentService, Resources.ARTIST)
                                                 ? "<a href='" + playlist.artist + "'>" + playlist.artist + "</a>"
                                                 : playlist.artist : qsTr("Unknown"))
                        onLinkActivated: appWindow.pageStack.push(Qt.resolvedUrl("PluginArtistPage.qml"))
                                                                 .load(playlist.artistId)
                    }
                    
                    Label {
                        width: parent.width
                        font.family: UI.FONT_FAMILY_LIGHT
                        font.pixelSize: UI.FONT_SMALL
                        elide: Text.ElideRight
                        text: qsTr("Date") + ": " + (playlist.date ? playlist.date : qsTr("Unknown"))
                    }
                    
                    Label {
                        width: parent.width
                        font.family: UI.FONT_FAMILY_LIGHT
                        font.pixelSize: UI.FONT_SMALL
                        elide: Text.ElideRight
                        text: qsTr("Genre") + ": " + (playlist.genre ? playlist.genre : qsTr("Unknown"))
                    }
                    
                    Label {
                        width: parent.width
                        font.family: UI.FONT_FAMILY_LIGHT
                        font.pixelSize: UI.FONT_SMALL
                        elide: Text.ElideRight
                        text: qsTr("Tracks") + ": " + playlist.trackCount
                    }
                }
            }
            
            Flickable {
                id: flicker

                anchors {
                    left: parent.left
                    right: parent.right
                    top: row.bottom
                    topMargin: UI.PADDING_DOUBLE
                    bottom: parent.bottom
                }
                contentHeight: descriptionLabel.height + UI.PADDING_DOUBLE
                clip: true
                
                Label {
                    id: descriptionLabel
                    
                    anchors {
                        left: parent.left
                        leftMargin: UI.PADDING_DOUBLE
                        right: parent.right
                        rightMargin: UI.PADDING_DOUBLE
                        top: parent.top
                    }
                    text: playlist.description ? Utils.toRichText(playlist.description) : qsTr("No description")
                    onLinkActivated: {
                        var resource = Resources.getResourceFromUrl(link);

                        if (resource.service != Settings.currentService) {
                            Qt.openUrlExternally(link);
                            return;
                        }

                        if (resource.type == Resources.ARTIST) {
                            appWindow.pageStack.push(Qt.resolvedUrl("PluginArtistPage.qml")).load(resource.id);
                        }
                        else if (resource.type == Resources.PLAYLIST) {
                            appWindow.pageStack.push(Qt.resolvedUrl("PluginPlaylistPage.qml")).load(resource.id);
                        }
                        else {
                            appWindow.pageStack.push(Qt.resolvedUrl("PluginTrackPage.qml")).load(resource.id);
                        }
                    }
                }
            }

            ScrollDecorator {
                flickableItem: flicker
            }

            states: State {
                name: "landscape"
                when: !appWindow.inPortrait

                AnchorChanges {
                    target: row
                    anchors.right: parent.horizontalCenter
                }

                AnchorChanges {
                    target: flicker
                    anchors {
                        left: row.right
                        top: header.bottom
                    }
                }
            }
        }

        PluginTracksTab {
            id: tracksTab

            width: view.width
            height: view.height
            title: qsTr("Tracks")
        }
    }

    Connections {
        target: tracksTab.model
        onStatusChanged: {
            if ((tracksTab.model.status == ResourcesRequest.Ready) && (tracksTab.model.canFetchMore)) {
                tracksTab.model.fetchMore();
            }
        }
    }
}
