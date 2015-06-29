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
    
    property alias model: activityModel
    property alias view: view
    
    title: qsTr("Stream")
    
    TableView {
        id: view
        
        anchors.fill: parent
        focus: true
        alternatingRowColors: false
        model: SoundCloudActivityModel {
            id: activityModel
            
            onStatusChanged: if (status == QSoundCloud.ResourcesRequest.Failed) messageBox.showError(errorString);
        }
        itemDelegate: SoundCloudActivityDelegate {
            onPressed: {
                view.forceActiveFocus();
                view.selection.clear();
                view.selection.select(styleData.row);
                view.currentRow = styleData.row;
            }
            onDoubleClicked: {
                if (activityModel.data(styleData.row, "activityType") == "playlist") {
                    pageStack.push({item: Qt.resolvedUrl("SoundCloudPlaylistPage.qml"), immediate: true})
                                  .loadPlaylist(activityModel.data(styleData.row, "originId"));
                }
                else {
                    pageStack.push({item: Qt.resolvedUrl("SoundCloudTrackPage.qml"), immediate: true})
                                  .loadTrack(activityModel.data(styleData.row, "originId"));
                }
            }
        }
        
        TableViewColumn {
            role: "title"
            title: qsTr("Title")
            width: Math.floor(root.width / 3)
        }
        
        TableViewColumn {
            role: "activityTypeString"
            title: qsTr("Type")
        }
        
        Keys.onReturnPressed: {
            if (currentRow >= 0) {
                if (activityModel.data(currentRow, "activityType") == "playlist") {
                    pageStack.push({item: Qt.resolvedUrl("SoundCloudPlaylistPage.qml"), immediate: true})
                                  .loadPlaylist(activityModel.data(currentRow, "originId"));
                }
                else {
                    pageStack.push({item: Qt.resolvedUrl("SoundCloudTrackPage.qml"), immediate: true})
                                  .loadTrack(activityModel.data(currentRow, "originId"));
                }
            }
        }
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
        
        SoundCloudActivityPopup {
            z: 1000
            activityId: activityModel.data(popupMouseArea.row, "id")
            thumbnail: activityModel.data(popupMouseArea.row, "originThumbnailUrl")
            title: activityModel.data(popupMouseArea.row, "title")
            type: activityModel.data(popupMouseArea.row, "activityTypeString");
            description: activityModel.data(popupMouseArea.row, "description")
            
            Component.onCompleted: {
                x = Math.min(popupMouseArea.mouseX + 2, popupMouseArea.width - width)
                y = Math.min(popupMouseArea.mouseY + 2, popupMouseArea.height - height);
            }
        }
    }
}
