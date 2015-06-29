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
    
    function loadTrack(trackOrId) {
        if (trackOrId.hasOwnProperty("id")) {
            track.loadTrack(trackOrId);
        }
        else {
            track.loadTrack(Settings.currentService, trackOrId);
        }
    }
    
    title: track.title ? track.title : qsTr("Track")
    
    PluginTrack {
        id: track
        
        onStatusChanged: if (status == ResourcesRequest.Failed) messageBox.showError(errorString);
    }
    
    TabView {
        id: tabView
        
        anchors.fill: parent
        frameVisible: false
        enabled: track.id != ""
        
        Tab {
            id: infoTab
            
            title: qsTr("Info")
            
            Page {
                id: infoPage
                
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
                    source: track.thumbnailUrl
                }
                
                ColumnLayout {
                    anchors {
                        left: thumbnail.left
                        right: thumbnail.right
                        top: thumbnail.bottom
                        topMargin: 10
                    }
                    
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Queue")
                        iconName: "list-add"
                        onClicked: player.addTrack(track)
                    }
                    
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Play")
                        iconName: "media-playback-start"
                        onClicked: player.playTrack(track)
                    }
                    
                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Download")
                        iconName: "folder-download"
                        enabled: track.downloadable
                        onClicked: {
                            loader.sourceComponent = downloadDialog;
                            loader.item.open();
                        }
                    }
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
                        text: qsTr("Title")
                    }
        
                    Label {            
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: track.title
                    }
                    
                    Label {
                        color: palette.mid
                        font.bold: true
                        text: qsTr("Artist")
                    }
        
                    Label {            
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: track.artist
                    }
                    
                    Label {
                        color: palette.mid
                        font.bold: true
                        text: qsTr("Genre")
                    }
        
                    Label {            
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: track.genre
                    }
                    
                    Label {
                        color: palette.mid
                        font.bold: true
                        text: qsTr("Date")
                    }
        
                    Label {            
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: track.date
                    }
                    
                    Label {
                        color: palette.mid
                        font.bold: true
                        text: qsTr("Format")
                    }
        
                    Label {            
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: track.format
                    }
                    
                    Label {
                        color: palette.mid
                        font.bold: true
                        text: qsTr("Length")
                    }
        
                    Label {            
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: track.durationString
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
                                text: Utils.toRichText(track.description)
                                onLinkActivated: {
                                    var resource = Resources.getResourceFromUrl(link);

                                    if (resource.service != track.service) {
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
            id: commentsTab
            
            title: qsTr("Comments")
            enabled: Plugins.resourceTypeIsSupported(track.service, Resources.COMMENT)
            
            ScrollView {
                anchors.fill: parent
                
                ListView {
                    id: commentView
                    
                    anchors.fill: parent
                    model: PluginCommentModel {
                        id: commentModel
                        
                        service: Settings.currentService
                        onStatusChanged: if (status == ResourcesRequest.Failed) messageBox.showError(errorString);
                    }
                    delegate: CommentDelegate {
                        onThumbnailClicked: pageStack.push({item: Qt.resolvedUrl("PluginArtistPage.qml"),
                                                            immediate: true}).loadArtist(commentModel.get(index))
                    }
                }
                
                Component.onCompleted: commentModel.list(track.id)
            }
        }        
    }
    
    Loader {
        id: loader
    }
    
    Component {
        id: downloadDialog
        
        PluginDownloadDialog {
            resourceId: track.id
            resourceTitle: track.title
        }
    }
}
