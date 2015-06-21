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
import ".."

Page {
    id: root
    
    function loadArtist(artistOrId) {
        if (artistOrId.hasOwnProperty("id")) {
            artist.loadArtist(artistOrId);
        }
        else {
            artist.loadArtist(Settings.currentService, artistOrId);
        }
    }
    
    title: artist.name ? artist.name : qsTr("Artist")
    
    PluginArtist {
        id: artist
        
        onStatusChanged: if (status == ResourcesRequest.Failed) messageBox.showError(errorString);
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

                                    if (resource.service != artist.id) {
                                        Qt.openUrlExternally(link);
                                        return;
                                    }

                                    if (resource.type == Resources.ARTIST) {
                                        pageStack.push({item: Qt.resolvedUrl("PluginArtistPage.qml"), immediate: true})
                                                      .loadArtist(resource.id);
                                    }
                                    else if (resource.type == Resources.PLAYLIST) {
                                        pageStack.push({item: Qt.resolvedUrl("PluginPlaylistPage.qml"), immediate: true})
                                                      .loadPlaylist(resource.id);
                                    }
                                    else {
                                        pageStack.push({item: Qt.resolvedUrl("PluginTrackPage.qml"), immediate: true})
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
            
            PluginTracksPage {
                id: tracksPage
                
                Component.onCompleted: model.list(artist.id)
            }
        }
        
        Tab {
            id: playlistsTab
            
            title: qsTr("Sets")
            enabled: Plugins.resourceTypeIsSupported(artist.service, Resources.PLAYLIST)
            
            PluginPlaylistsPage {
                id: playlistsPage
                
                Component.onCompleted: model.list(artist.id)
            }
        }
    }
}
