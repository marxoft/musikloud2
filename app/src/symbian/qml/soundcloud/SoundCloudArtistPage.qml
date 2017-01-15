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
    
    function load(artistOrId) {
        artist.loadArtist(artistOrId);

        if ((Qt.isQtObject(artistOrId)) && (!artist.followed) && (soundcloud.userId !== "")) {
            artist.checkIfFollowed();
        }
    }
    
    title: artist.name ? artist.name : qsTr("Artist")
    tools: ToolBarLayout {
        BackToolButton {
            onClicked: artist.cancel()
        }
        
        NowPlayingButton {}
        
        MyToolButton {            
            iconSource: "toolbar-view-menu"
            toolTip: qsTr("Options")
            onClicked: popups.open(menu, root)
        }
    }
    
    SoundCloudArtist {
        id: artist
        
        onStatusChanged: {
            switch (status) {
            case QSoundCloud.ResourcesRequest.Loading:
                root.showProgressIndicator = true;
                return;
            case QSoundCloud.ResourcesRequest.Ready:
                if ((!followed) && (soundcloud.userId !== "")) {
                    root.showProgressIndicator = false;
                    checkIfFollowed();
                    return;
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
                text: artist.name ? artist.name : qsTr("Unknown artist")
            }
            
            Avatar {
                id: avatar
                
                width: platformStyle.graphicSizeLarge
                height: platformStyle.graphicSizeLarge
                source: artist.thumbnailUrl
                placeholderSource: "../images/artist.jpg"
            }
            
            Label {
                id: descriptionLabel
                
                width: parent.width
                wrapMode: Text.Wrap
                text: artist.description ? utils.toRichText(artist.description) : qsTr("No description")
                onLinkActivated: {
                    var resource = mkresources.getResourceFromUrl(link);
                    
                    if (resource.service !== artist.service) {
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
                    text: artist.followed ? qsTr("Unfollow") : qsTr("Follow")
                    enabled: (soundcloud.userId) && (soundcloud.userId !== artist.id)
                    onClicked: artist.followed ? artist.unfollow() : artist.follow()
                }
                
                MenuItem {
                    text: qsTr("Tracks")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudTracksPage.qml"), {title: text})
                    .get("/users/" + artist.id + "/tracks", {limit: MAX_RESULTS})
                }
                
                MenuItem {
                    text: qsTr("Favourites")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudTracksPage.qml"), {title: text})
                    .get("/users/" + artist.id + "/favorites", {limit: MAX_RESULTS})
                }
                
                MenuItem {
                    text: qsTr("Sets")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudPlaylistsPage.qml"), {title: text})
                    .get("/users/" + artist.id + "/playlists", {limit: MAX_RESULTS})
                }
                
                MenuItem {
                    text: qsTr("Followings")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudArtistsPage.qml"), {title: text})
                    .get("/users/" + artist.id + "/followings", {limit: MAX_RESULTS})
                }
            }
        }
    }
}
