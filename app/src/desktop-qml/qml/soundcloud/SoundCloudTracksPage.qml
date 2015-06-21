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
import MusiKloud 2.0
import QSoundCloud 1.0 as QSoundCloud
import ".."

Page {
    id: root
    
    property alias model: trackModel
    property alias view: view
    
    title: qsTr("Tracks")
    
    TableView {
        id: view
        
        anchors.fill: parent
        focus: true
        alternatingRowColors: false
        model: SoundCloudTrackModel {
            id: trackModel
            
            onStatusChanged: if (status == QSoundCloud.ResourcesRequest.Failed) messageBox.showError(errorString);
        }
        itemDelegate: TrackDelegate {
            onPressed: {
                view.forceActiveFocus();
                view.selection.clear();
                view.selection.select(styleData.row);
                view.currentRow = styleData.row;
            }
            onDoubleClicked: player.playTrack(trackModel.get(styleData.row))
            onRightClicked: contextMenu.popup()
        }
                
        TableViewColumn {
            role: "title"
            title: qsTr("Title")
            width: Math.floor(root.width / 3)
        }
        
        TableViewColumn {
            role: "artist"
            title: qsTr("Artist")
        }
        
        TableViewColumn {
            role: "date"
            title: qsTr("Date")
        }
        
        TableViewColumn {
            role: "durationString"
            title: qsTr("Length")
        }
        
        Keys.onReturnPressed: if (currentRow >= 0) pageStack.push({item: Qt.resolvedUrl("SoundCloudTrackPage.qml"), immediate: true})
                                                                 .loadTrack(trackModel.get(currentRow));
    }
    
    MouseArea {
        id: popupMouseArea
        
        property int row: -1
        
        anchors.fill: view
        hoverEnabled: true
        enabled: view.rowCount > 0
        onMouseYChanged: row = Math.max(0, view.rowAt(10, mouseY))
        onExited: {
            loader.sourceComponent = undefined;
            popupTimer.stop();
        }
        onRowChanged: {
            loader.sourceComponent = undefined;
            popupTimer.restart();
        }
        onPressed: mouse.accepted = false
    }
    
    Menu {
        id: contextMenu
        
        MenuItem {
            text: qsTr("View info")
            iconName: "help-info"
            onTriggered: pageStack.push({item: Qt.resolvedUrl("SoundCloudTrackPage.qml"), immediate: true})
                                       .loadTrack(trackModel.get(view.currentRow))
        }
        
        MenuItem {
            text: qsTr("Queue")
            iconName: "list-add"
            onTriggered: player.addTrack(trackModel.get(view.currentRow))
        }
        
        MenuItem {
            text: qsTr("Download")
            iconName: "folder-download"
            onTriggered: {
                loader.sourceComponent = downloadDialog;
                loader.item.open();
            }
        }
        
        MenuItem {
            text: (view.currentRow >= 0) && (trackModel.data(view.currentRow, "favourited")) ? qsTr("Unfavorite")
                                                                                            : qsTr("Favourite")
            
            iconName: (view.currentRow >= 0) && (trackModel.data(view.currentRow, "favourited")) ? "dialog-no" 
                                                                                                : "dialog-yes"
            enabled: SoundCloud.userId != ""
            onTriggered: {
                var track = trackModel.get(view.currentRow);
                
                if (track.favourited) {
                    track.unfavourite();
                }
                else {
                    track.favourite();
                }
            }
        }
    }

    Loader {
        id: loader        
    }
    
    Timer {
        id: popupTimer
        
        interval: 500
        onTriggered: loader.sourceComponent = popup
    }
    
    Component {
        id: popup
        
        TrackPopup {
            z: 1000
            trackId: trackModel.data(popupMouseArea.row, "id")
            thumbnail: trackModel.data(popupMouseArea.row, "thumbnailUrl")
            title: trackModel.data(popupMouseArea.row, "title")
            artist: trackModel.data(popupMouseArea.row, "artist")
            genre: trackModel.data(popupMouseArea.row, "genre")
            date: trackModel.data(popupMouseArea.row, "date")
            format: trackModel.data(popupMouseArea.row, "format")
            duration: trackModel.data(popupMouseArea.row, "durationString")
            
            Component.onCompleted: {
                x = Math.min(popupMouseArea.mouseX + 2, popupMouseArea.width - width)
                y = Math.min(popupMouseArea.mouseY + 2, popupMouseArea.height - height);
            }
        }
    }
    
    Component {
        id: downloadDialog
        
        SoundCloudDownloadDialog {
            resourceId: trackModel.data(view.currentRow, "id")
            resourceTitle: trackModel.data(view.currentRow, "title")
        }
    }
}
