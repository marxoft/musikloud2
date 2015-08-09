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
import QSoundCloud 1.0 as QSoundCloud
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    function load(artistOrId) {
        artist.loadArtist(artistOrId);

        if ((artist.id) && (!artist.followed) && (SoundCloud.userId)) {
            artist.checkIfFollowed();
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

    SoundCloudArtist {
        id: artist

        onStatusChanged: {
            switch (status) {
            case QSoundCloud.ResourcesRequest.Ready:
                if ((!followed) && (SoundCloud.userId)) {
                    checkIfFollowed();
                }

                break;
            case QSoundCloud.ResourcesRequest.Failed:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }
        }
    }

    TabView {
        id: view

        anchors.fill: parent
        enabled: artist.id != ""

        Tab {
            id: infoTab

            width: view.width
            height: view.height
            tools: ToolBarLayout {

                BackToolIcon {}
                
                NowPlayingButton {}

                MyToolIcon {
                    platformIconId: artist.followed ? "toolbar-cancle" : "toolbar-add"
                    enabled: (SoundCloud.userId) && (artist.id)
                             && (artist.id != SoundCloud.userId)
                    onClicked: artist.followed ? artist.unfollow() : artist.follow()
                }
            }
            
            PageHeader {
                id: header
                
                title: artist.name
                showProgressIndicator: artist.status == QSoundCloud.ResourcesRequest.Loading
            }

            Flow {
                id: flow

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

                Item {
                    width: parent.width - avatar.width - UI.PADDING_DOUBLE
                    height: avatar.height
                }

                Label {
                    id: nameLabel

                    font.bold: true
                    text: artist.name
                }

                Label {
                    id: statsLabel

                    width: parent.width - nameLabel.width - UI.PADDING_DOUBLE
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: UI.FONT_SMALL
                    font.family: UI.FONT_FAMILY_LIGHT
                    text: Utils.formatLargeNumber(artist.followersCount) + " " + qsTr("followers")
                }
            }

            Flickable {
                id: flicker

                anchors {
                    left: parent.left
                    right: parent.right
                    top: flow.bottom
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

                        if (resource.service != Resources.SOUNDCLOUD) {
                            Qt.openUrlExternally(link);
                            return;
                        }

                        if (resource.type == Resources.ARTIST) {
                            appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudArtistPage.qml")).load(resource.id);
                        }
                        else if (resource.type == Resources.PLAYLIST) {
                            appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudPlaylistPage.qml")).load(resource.id);
                        }
                        else {
                            appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudTrackPage.qml")).load(resource.id);
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
                    target: flow
                    anchors.right: parent.horizontalCenter
                }

                AnchorChanges {
                    target: flicker
                    anchors {
                        left: flow.right
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
                SoundCloudTracksTab {
                    title: artist.name + "'s " + qsTr("tracks")
                    Component.onCompleted: model.get("/users/" + artist.id + "/tracks", {limit: MAX_RESULTS})
                }
            }
        }

        TabLoader {
            id: favouritesTab

            width: view.width
            height: view.height
            title: artist.name + "'s " + qsTr("favourites")
            tab: Component {
                SoundCloudTracksTab {
                    title: artist.name + "'s " + qsTr("favourites")
                    Component.onCompleted: model.get("/users/" + artist.id + "/favorites", {limit: MAX_RESULTS})
                }
            }
        }

        TabLoader {
            id: playlistsTab

            width: view.width
            height: view.height
            title: artist.name + "'s " + qsTr("sets")
            tab: Component {
                SoundCloudPlaylistsTab {
                    title: artist.name + "'s " + qsTr("sets")
                    Component.onCompleted: model.get("/users/" + artist.id + "/playlists", {limit: MAX_RESULTS})
                }
            }
        }

        TabLoader {
            id: followingsTab

            width: view.width
            height: view.height
            title: artist.name + "'s " + qsTr("followings")
            tab: Component {
                SoundCloudArtistsTab {
                    title: artist.name + "'s " + qsTr("followings")
                    Component.onCompleted: model.get("/users/" + artist.id + "/followings", {limit: MAX_RESULTS})
                }
            }
        }
    }
}
