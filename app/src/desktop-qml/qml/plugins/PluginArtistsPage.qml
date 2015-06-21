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
    
    property alias model: artistModel
    property alias view: view
    
    title: qsTr("Artists")
    
    TableView {
        id: view
        
        anchors.fill: parent
        focus: true
        alternatingRowColors: false
        model: PluginArtistModel {
            id: artistModel
            
            service: Settings.currentService
        }
        itemDelegate: ArtistDelegate {
            onPressed: {
                view.forceActiveFocus();
                view.selection.clear();
                view.selection.select(styleData.row);
                view.currentRow = styleData.row;
            }
            onDoubleClicked: pageStack.push({item: Qt.resolvedUrl("PluginArtistPage.qml"), immediate: true})
                                           .loadArtist(artistModel.get(styleData.row))
        }
                
        TableViewColumn {
            role: "name"
            title: qsTr("Name")
        }
        
        Keys.onReturnPressed: if (currentRow >= 0) pageStack.push({item: Qt.resolvedUrl("PluginArtistPage.qml"), immediate: true})
                                                                 .loadArtist(artistModel.get(currentRow));
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
        
        ArtistPopup {
            z: 1000
            artistId: artistModel.data(popupMouseArea.row, "id")
            thumbnail: artistModel.data(popupMouseArea.row, "thumbnailUrl")
            name: artistModel.data(popupMouseArea.row, "name")
            description: artistModel.data(popupMouseArea.row, "description")
            
            Component.onCompleted: {
                x = Math.min(popupMouseArea.mouseX + 2, popupMouseArea.width - width)
                y = Math.min(popupMouseArea.mouseY + 2, popupMouseArea.height - height);
            }
        }
    }
}
