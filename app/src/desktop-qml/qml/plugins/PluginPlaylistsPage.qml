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
import ".."

Page {
    id: root
    
    property alias model: playlistModel
    property alias view: view
    
    title: qsTr("Playlists")
    
    TableView {
        id: view
        
        anchors.fill: parent
        focus: true
        alternatingRowColors: false
        model: PluginPlaylistModel {
            id: playlistModel
            
            service: Settings.currentService
            onStatusChanged: if (status == ResourcesRequest.Failed) messageBox.showError(errorString);
        }
        itemDelegate: PlaylistDelegate {
            onPressed: {
                view.forceActiveFocus();
                view.selection.clear();
                view.selection.select(styleData.row);
                view.currentRow = styleData.row;
            }
            onDoubleClicked: pageStack.push({item: Qt.resolvedUrl("PluginPlaylistPage.qml"), immediate: true})
                                           .loadPlaylist(playlistModel.get(styleData.row))
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
            role: "trackCount"
            title: qsTr("Tracks")
        }
        
        Keys.onReturnPressed: if (currentRow >= 0) pageStack.push({item: Qt.resolvedUrl("PluginPlaylistPage.qml"), immediate: true})
                                                                 .loadPlaylist(playlistModel.get(currentRow));
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
        
        PlaylistPopup {
            z: 1000
            playlistId: playlistModel.data(popupMouseArea.row, "id")
            thumbnail: playlistModel.data(popupMouseArea.row, "thumbnailUrl")
            title: playlistModel.data(popupMouseArea.row, "title")
            artist: playlistModel.data(popupMouseArea.row, "artist")
            genre: playlistModel.data(popupMouseArea.row, "genre")
            date: playlistModel.data(popupMouseArea.row, "date")
            trackCount: playlistModel.data(popupMouseArea.row, "trackCount")
            duration: playlistModel.data(popupMouseArea.row, "durationString")
            
            Component.onCompleted: {
                x = Math.min(popupMouseArea.mouseX + 2, popupMouseArea.width - width)
                y = Math.min(popupMouseArea.mouseY + 2, popupMouseArea.height - height);
            }
        }
    }
}
