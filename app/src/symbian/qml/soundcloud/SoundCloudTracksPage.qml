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
    
    function get(path, filters) {
        trackModel.get(path, filters);
    }
    
    title: qsTr("Tracks")
    tools: ToolBarLayout {
        BackToolButton {
            onClicked: trackModel.cancel()
        }
        
        NowPlayingButton {}
        
        MyToolButton {
            id: reloadButton
            
            iconSource: "toolbar-refresh"
            toolTip: qsTr("Reload")
            onClicked: trackModel.reload()
        }
    }
    
    MyListView {
        id: view

        anchors.fill: parent
        model: SoundCloudTrackModel {
            id: trackModel
            
            onStatusChanged: {
                switch (status) {
                case QSoundCloud.ResourcesRequest.Loading: {
                    root.showProgressIndicator = true;
                    reloadButton.enabled = false;
                    label.visible = false;
                    return;
                }
                case QSoundCloud.ResourcesRequest.Failed:
                    infoBanner.information(errorString);
                    break;
                default:
                    break;
                }
                
                root.showProgressIndicator = false;
                reloadButton.enabled = true;
                label.visible = (count === 0);
            }
        }
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
    
    Label {
        id: label
        
        anchors {
            fill: parent
            margins: platformStyle.paddingLarge
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        color: platformStyle.colorNormalMid
        font.bold: true
        font.pixelSize: 32
        text: qsTr("No results")
        visible: false
    }
    
    PopupLoader {
        id: popups
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
