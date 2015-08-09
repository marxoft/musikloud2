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

    function load(trackOrId) {
        if (trackOrId.hasOwnProperty("id")) {
            track.loadTrack(trackOrId);
        }
        else {
            track.loadTrack(Settings.currentService, trackOrId);
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

    PluginTrack {
        id: track

        onStatusChanged: if (status == QSoundCloud.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
    }

    TabView {
        id: view

        anchors.fill: parent
        enabled: track.id != ""

        Tab {
            id: infoTab

            width: view.width
            height: view.height
            tools: ToolBarLayout {
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
                        onClicked: {
                            player.addTrack(track);
                            infoBanner.showMessage(qsTr("1 track added to queue"));
                        }
                    }

                    MenuItem {
                        text: qsTr("Download")
                        enabled: track.downloadable
                        onClicked: {
                            dialogLoader.sourceComponent = downloadDialog;
                            dialogLoader.item.resourceId = track.id;
                            dialogLoader.item.resourceTitle = track.title;
                            dialogLoader.item.streamUrl = track.streamUrl;
                            dialogLoader.item.open();
                        }
                    }

                    MenuItem {
                        text: qsTr("Share")
                        onClicked: if (!ShareUi.shareTrack(track)) infoBanner.showMessage(qsTr("Unable to share track"));
                    }
                }
            }
            
            PageHeader {
                id: header
                
                title: track.title
                showProgressIndicator: track.status == ResourcesRequest.Loading
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
                    source: track.largeThumbnailUrl
                    placeholderText: track.title
                    onClicked: {
                        player.clearQueue();
                        player.addTrack(track);
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
                              + (track.artist ? Plugins.resourceTypeIsSupported(Settings.currentService, Resources.ARTIST)
                                              ? "<a href='" + track.artist + "'>" + track.artist + "</a>"
                                              : track.artist : qsTr("Unknown"))
                        onLinkActivated: appWindow.pageStack.push(Qt.resolvedUrl("PluginArtistPage.qml"))
                                                                 .load(track.artistId)
                    }
                    
                    Label {
                        width: parent.width
                        font.family: UI.FONT_FAMILY_LIGHT
                        font.pixelSize: UI.FONT_SMALL
                        elide: Text.ElideRight
                        text: qsTr("Date") + ": " + (track.date ? track.date : qsTr("Unknown"))
                    }
                    
                    Label {
                        width: parent.width
                        font.family: UI.FONT_FAMILY_LIGHT
                        font.pixelSize: UI.FONT_SMALL
                        elide: Text.ElideRight
                        text: qsTr("Duration") + ": " + track.durationString
                    }
                    
                    Label {
                        width: parent.width
                        font.family: UI.FONT_FAMILY_LIGHT
                        font.pixelSize: UI.FONT_SMALL
                        elide: Text.ElideRight
                        text: qsTr("Genre") + ": " + (track.genre ? track.genre : qsTr("Unknown"))
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
                    text: track.description ? Utils.toRichText(track.description) : qsTr("No description")
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

            Loader {
                id: dialogLoader
            }

            Component {
                id: downloadDialog

                PluginDownloadDialog {}
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

        TabLoader {
            id: relatedTab

            width: view.width
            height: view.height
            title: qsTr("Related")
            tab: Component {
                PluginTracksTab {
                    title: qsTr("Related")
                    Component.onCompleted: model.list(track.id)
                }
            }
        }

        TabLoader {
            id: commentsTab

            width: view.width
            height: view.height
            title: qsTr("Comments")
            tab: Component {
                PluginCommentsTab {
                    title: qsTr("Comments")
                    Component.onCompleted: {
                        if (Plugins.resourceTypeIsSupported(Settings.currentService, Resources.COMMENT)) {
                            model.list(track.id);
                        }
                        else {
                            infoBanner.showMessage(qsTr("This track does not have any comments"));
                        }
                    }
                }
            }
        }
    }
}
