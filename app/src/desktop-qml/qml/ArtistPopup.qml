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

Rectangle {
    id: root
    
    property string artistId
    property alias thumbnail: thumbnail.source
    property alias name: nameLabel.text
    property alias description: descriptionLabel.text
    
    z: 1000
    width: 400
    height: Math.max(thumbnail.height, grid.height) + 20
    color: palette.window
    
    MouseArea {
        anchors.fill: parent
    }
    
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
    }
    
    GridLayout {
        id: grid
        
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
            id: nameLabel
            
            Layout.fillWidth: true
            elide: Text.ElideRight
        }
        
        Label {
            Layout.alignment: Qt.AlignTop
            color: palette.mid
            font.bold: true
            text: qsTr("Description")
        }
        
        Label {
            id: descriptionLabel
            
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            maximumLineCount: 10
            elide: Text.ElideRight
        }
    }
}
