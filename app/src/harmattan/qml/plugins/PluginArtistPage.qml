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

    function load(artistOrId) {
        if (artistOrId.hasOwnProperty("id")) {
            artist.loadArtist(artistOrId);
        }
        else {
            artist.loadArtist(Settings.currentService, artistOrId);
        }
    }

    tools: view.currentTab.tools ? view.currentTab.tools : ownTools
    onToolsChanged: if (status == PageStatus.Active) appWindow.pageStack.toolBar.setTools(tools, "set");

    ToolBarLayout {
        id: ownTools

        visible: false

        BackToolIcon {}
        
        NowPlayingButton {}
    }

    PluginArtist {
        id: artist

        onStatusChanged: if (status == ResourcesRequest.Failed) infoBanner.showMessage(errorString);
    }

    TabView {
        id: view

        anchors.fill: parent
        enabled: artist.id != ""

        Tab {
            id: infoTab

            width: view.width
            height: view.height
            
            PageHeader {
                id: header
                
                title: artist.name
                showProgressIndicator: artist.status == ResourcesRequest.Loading
            }

            Column {
                id: column

                anchors {
                    left: parent.left
                    right: parent.right
                    top: header.bottom
                    margins: UI.PADDING_DOUBLE
                }

                spacing: UI.PADDING_DOUBLE

                Avatar {
                    id: avatar

                    width: height
                    height: Math.floor(parent.width / 4)
                    source: artist.thumbnailUrl
                    enabled: false
                }

                Label {
                    id: nameLabel

                    width: parent.width
                    font.bold: true
                    text: artist.name
                }
            }

            Flickable {
                id: flicker

                anchors {
                    left: parent.left
                    right: parent.right
                    top: column.bottom
                    topMargin: UI.PADDING_DOUBLE
                    bottom: parent.bottom
                }
                clip: true
                contentHeight: descriptionLabel.height + UI.PADDING_DOUBLE * 2

                Label {
                    id: descriptionLabel

                    anchors {
                        left: parent.left
                        leftMargin: UI.PADDING_DOUBLE
                        right: parent.right
                        rightMargin: UI.PADDING_DOUBLE
                        top: parent.top
                    }
                    text: artist.description ? Utils.toRichText(artist.description) : qsTr("No description")
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
                    target: column
                    anchors.right: parent.horizontalCenter
                }

                AnchorChanges {
                    target: flicker
                    anchors {
                        left: column.right
                        top: header.bottom
                    }
                }
            }
        }

        TabLoader {
            id: tracksTab

            width: view.width
            height: view.height
            title: artist.name + "'s " + qsTr("tracks")
            tab: Component {
                PluginTracksTab {
                    title: artist.name + "'s " + qsTr("tracks")
                    Component.onCompleted: model.list(artist.id)
                }
            }
        }

        TabLoader {
            id: playlistsTab

            width: view.width
            height: view.height
            title: artist.name + "'s " + qsTr("playlists")
            tab: Component {
                PluginPlaylistsTab {
                    title: artist.name + "'s " + qsTr("playlists")
                    Component.onCompleted: {
                        if (Plugins.resourceTypeIsSupported(Settings.currentService, Resources.PLAYLIST)) {
                            model.list(artist.id);
                        }
                        else {
                            infoBanner.showMessage(qsTr("This artist does not have any playlists"));
                        }
                    }
                }
            }
        }
    }
}
