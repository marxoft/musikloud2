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

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import MusiKloud 2.0
import QSoundCloud 1.0 as QSoundCloud
import ".."

Page {
    id: root
    
    function loadArtist(artistOrId) {
        artist.loadArtist(artistOrId);
        
        if ((artist.id) && (!artist.followed) && (SoundCloud.userId)) {
            artist.checkIfFollowed();
        }
    }
    
    title: artist.name ? artist.name : qsTr("User")
    
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
                messageBox.showError(errorString);
                break;
            default:
                break;
            }
        }
    }
    
    TabView {
        id: tabView
        
        anchors.fill: parent
        frameVisible: false
        enabled: artist.id != ""
        
        Tab {
            id: profileTab
            
            title: qsTr("Profile")
            
            Page {
                id: profilePage
                
                Image {
                    id: thumbnail
        
                    width: height
                    height: 100
                    anchors {
                        left: parent.left
                        top: parent.top
                        margins: 10
                    }
                    smooth: true
                    source: artist.thumbnailUrl
                }
                
                Button {
                    id: followButton
                    
                    anchors {
                        left: thumbnail.left
                        right: thumbnail.right
                        top: thumbnail.bottom
                        topMargin: 10
                    }
                    
                    text: artist.followed ? qsTr("Followed") : qsTr("Follow")
                    iconName: artist.followed ? "dialog-yes" : ""
                    tooltip: (artist.followed ? qsTr("Unfollow") : qsTr("Follow")) + " " + artist.name
                    enabled: (SoundCloud.userId) && (artist.id) && (artist.id != SoundCloud.userId)
                    onClicked: artist.followed ? artist.unfollow() : artist.follow()
                }
    
                GridLayout {        
                    width: parent.width - thumbnail.width - 30
                    anchors {
                        right: parent.right
                        top: parent.top
                        margins: 10
                    }
                    columns: 2
        
                    Label {
                        color: palette.mid
                        font.bold: true
                        text: qsTr("Name")
                    }
        
                    Label {            
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: artist.name
                    }
                    
                    Label {
                        color: palette.mid
                        font.bold: true
                        text: qsTr("Tracks")
                    }
        
                    Label {            
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: artist.trackCount
                    }
                    
                    Label {
                        color: palette.mid
                        font.bold: true
                        text: qsTr("Sets")
                    }
        
                    Label {            
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: artist.playlistCount
                    }
                    
                    Label {
                        color: palette.mid
                        font.bold: true
                        text: qsTr("Followers")
                    }
        
                    Label {            
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: artist.followersCount
                    }
        
                    Label {
                        Layout.alignment: Qt.AlignTop
                        color: palette.mid
                        font.bold: true
                        text: qsTr("Description")
                    }
                    
                    ScrollView {
                        Layout.fillWidth: true
                        
                        Flickable {
                            anchors.fill: parent
                            contentHeight: descriptionLabel.height
                            
                            Label {
                                id: descriptionLabel
                               
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    top: parent.top
                                }
                                wrapMode: Text.Wrap
                                text: Utils.toRichText(artist.description)
                                onLinkActivated: {
                                    var resource = Resources.getResourceFromUrl(link);

                                    if (resource.service != Resources.SOUNDCLOUD) {
                                        Qt.openUrlExternally(link);
                                        return;
                                    }

                                    if (resource.type == Resources.ARTIST) {
                                        pageStack.push({item: Qt.resolvedUrl("SoundCloudArtistPage.qml"), immediate: true})
                                                      .loadArtist(resource.id);
                                    }
                                    else if (resource.type == Resources.PLAYLIST) {
                                        pageStack.push({item: Qt.resolvedUrl("SoundCloudPlaylistPage.qml"), immediate: true})
                                                      .loadPlaylist(resource.id);
                                    }
                                    else {
                                        pageStack.push({item: Qt.resolvedUrl("SoundCloudTrackPage.qml"), immediate: true})
                                                      .loadTrack(resource.id);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        Tab {
            id: tracksTab
            
            title: qsTr("Tracks")
            
            SoundCloudTracksPage {
                id: tracksPage
                
                Component.onCompleted: model.get("/users/" + artist.id + "/tracks", {limit: MAX_RESULTS})
            }
        }
        
        Tab {
            id: favouritesTab
            
            title: qsTr("Favourites")
            
            SoundCloudTracksPage {
                id: favouritesPage
                
                Component.onCompleted: model.get("/users/" + artist.id + "/favorites", {limit: MAX_RESULTS})
            }
        }
        
        Tab {
            id: playlistsTab
            
            title: qsTr("Sets")
            
            SoundCloudPlaylistsPage {
                id: playlistsPage
                
                Component.onCompleted: model.get("/users/" + artist.id + "/playlists", {limit: MAX_RESULTS})
            }
        }
        
        Tab {
            id: followingsTab
            
            title: qsTr("Followings")
            
            SoundCloudArtistsPage {
                id: followingsPage
                
                Component.onCompleted: model.get("/users/" + artist.id + "/followings", {limit: MAX_RESULTS})
            }
        }
    }
}
