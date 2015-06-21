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
import QtQml 2.2
import QtQuick.Controls 1.1

Page {
    id: root
    
    property alias model: view.model
    property alias view: view
    
    tools: ToolBarLayout {
        
        Label {
            text: qsTr("Playback queue") + " (" + player.queueCount + ")"
        }
    }
    
    TableView {
        id: view
        
        anchors.fill: parent
        focus: true
        alternatingRowColors: false
        model: player.queue
        itemDelegate: LabelDelegate {
            font.bold: styleData.row == player.currentIndex
            text: styleData.column == 0 ? styleData.row + 1 : styleData.value
            onPressed: {
                view.forceActiveFocus();
                view.selection.clear();
                view.selection.select(styleData.row);
                view.currentRow = styleData.row;
            }
            onDoubleClicked: player.currentIndex = styleData.row
            //onRightClicked: contextMenu.popup() Disabled as 'styleData.row' is not updated when an item is removed
            //                                    from the model.
        }
        
        TableViewColumn {
            title: qsTr("#")
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
            role: "genre"
            title: qsTr("Genre")
        }
        
        TableViewColumn {
            role: "durationString"
            title: qsTr("Length")
        }
        
        Keys.onReturnPressed: player.currentIndex = currentRow
    }
    
    MouseArea {
        id: popupMouseArea
        
        property int row: -1
        
        anchors.fill: view
        hoverEnabled: true
        onMouseYChanged: row = view.rowAt(10, mouseY)
        onExited: {
            loader.sourceComponent = undefined;
            popupTimer.stop();
        }
        onRowChanged: {
            loader.sourceComponent = undefined;
            
            if (row >= 0) {
                popupTimer.restart();
            }
        }
        onPressed: mouse.accepted = false
    }
    
    Menu {
        id: contextMenu
        
        MenuItem {
            text: qsTr("Remove")
            iconName: "edit-delete"
            onTriggered: player.removeTrack(view.currentRow)
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
            trackId: player.queue.data(Math.max(0, popupMouseArea.row), "id")
            thumbnail: player.queue.data(Math.max(0, popupMouseArea.row), "thumbnailUrl")
            title: player.queue.data(Math.max(0, popupMouseArea.row), "title")
            artist: player.queue.data(Math.max(0, popupMouseArea.row), "artist")
            genre: player.queue.data(Math.max(0, popupMouseArea.row), "genre")
            date: player.queue.data(Math.max(0, popupMouseArea.row), "date")
            format: player.queue.data(Math.max(0, popupMouseArea.row), "format")
            duration: player.queue.data(Math.max(0, popupMouseArea.row), "durationString")
            
            Component.onCompleted: {
                x = Math.min(popupMouseArea.mouseX + 2, popupMouseArea.width - width)
                y = Math.min(popupMouseArea.mouseY + 2, popupMouseArea.height - height);
            }
        }
    }
    
    Connections {
        target: player
        onCurrentIndexChanged: {
            view.selection.clear();
            view.selection.select(i);
            view.currentRow = i;
        }
    }
}
