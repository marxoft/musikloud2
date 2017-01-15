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
import ".."

MyPage {
    id: root
    
    function load(trackOrId) {
        if (Qt.isQtObject(trackOrId)) {
            track.loadTrack(trackOrId);
        }
        else {
            track.loadTrack(settings.currentService, trackOrId);
        }
    }
    
    title: track.title ? track.title : qsTr("Track")
    tools: ToolBarLayout {
        BackToolButton {
            onClicked: track.cancel()
        }
        
        NowPlayingButton {}
        
        MyToolButton {            
            iconSource: "toolbar-view-menu"
            toolTip: qsTr("Options")
            onClicked: popups.open(menu, root)
        }
    }
    
    PluginTrack {
        id: track
        
        onStatusChanged: {
            switch (status) {
            case ResourcesRequest.Loading:
                root.showProgressIndicator = true;
                return;
            case ResourcesRequest.Failed:
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
                text: track.title ? track.title : qsTr("Unknown title")
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
                source: track.thumbnailUrl
                placeholderSource: "../images/track.jpg"
                onClicked: media.playTrack(track)
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
                    text: qsTr("Artist") + ": " + (track.artistId ? "<a href='" + track.artistId + "'>"
                                                   + track.artist + "</a>" : qsTr("Unknown"))
                    onLinkActivated: appWindow.pageStack.push(Qt.resolvedUrl("PluginArtistPage.qml"))
                    .load(track.artistId)
                }
                
                Label {
                    id: genreLabel
                    
                    width: parent.width
                    elide: Text.ElideRight
                    text: qsTr("Genre") + ": " + (track.genre ? track.genre : qsTr("Unknown"))
                }
                
                Label {
                    id: durationLabel
                    
                    width: parent.width
                    elide: Text.ElideRight
                    text: qsTr("Duration") + ": " + (track.durationString ? track.durationString : qsTr("Unknown"))
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
                text: track.description ? utils.toRichText(track.description) : qsTr("No description")
                onLinkActivated: {
                    var resource = mkresources.getResourceFromUrl(link);
                    
                    if (resource.service !== track.service) {
                        Qt.openUrlExternally(link);
                        return;
                    }
                    
                    switch (resource.type) {
                    case mkresources.ARTIST:
                        appWindow.pageStack.push(Qt.resolvedUrl("PluginArtistPage.qml")).load(resource.id);
                        break;
                    case mkresources.PLAYLIST:
                        appWindow.pageStack.push(Qt.resolvedUrl("PluginPlaylistPage.qml")).load(resource.id);
                        break;
                    case mkresources.TRACK:
                        appWindow.pageStack.push(Qt.resolvedUrl("PluginTrackPage.qml")).load(resource.id);
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
                    onClicked: media.playTrack(track)
                }
                
                MenuItem {
                    text: qsTr("Queue")
                    onClicked: media.addTrack(track)
                }
                
                MenuItem {
                    text: qsTr("Download")
                    enabled: track.downloadable
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PluginDownloadPage.qml")).list(track)
                }

                MenuItem {
                    text: qsTr("Copy URL")
                    onClicked: {
                        clipboard.text = track.url;
                        infoBanner.information(qsTr("URL copied to clipboard"));
                    }
                }
                
                Repeater {
                    model: track.actions
                    
                    MenuItem {
                        text: modelData.label
                        onClicked: {
                            switch (modelData.method) {
                            case "del":
                                track.del(modelData.type, modelData.id);
                                break;
                            case "insert":
                                track.insert(modelData.type, modelData.id);
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }
                
                MenuItem {
                    text: qsTr("Related")
                    enabled: track.relatedTracksId !== ""
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PluginTracksPage.qml"), {title: text})
                    .list(track.relatedTracksId)
                }
                
                MenuItem {
                    text: qsTr("Comments")
                    enabled: track.commentsId !== ""
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PluginCommentsPage.qml"), {title: text})
                    .list(track.commentsId)
                }
            }
        }
    }
}
