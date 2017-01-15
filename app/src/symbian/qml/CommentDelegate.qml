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

Item {
    id: root
    
    signal activated
    signal clicked
    signal pressAndHold
    
    width: ListView.view ? ListView.view.width : screen.width
    height: avatar.height + bodyLabel.height + platformStyle.paddingLarge * 3
    
    Avatar {
        id: avatar
        
        anchors {
            left: parent.left
            top: parent.top
            margins: platformStyle.paddingLarge
        }
        width: platformStyle.graphicSizeSmall
        height: platformStyle.graphicSizeSmall
        source: thumbnailUrl
        placeholderSource: "images/artist.jpg"
        enabled: artistId !== ""
        onClicked: root.activated();
    }
    
    Label {
        id: artistLabel
        
        anchors {
            left: avatar.right
            right: parent.right
            verticalCenter: avatar.verticalCenter
            margins: platformStyle.paddingLarge
        }
        font.pixelSize: platformStyle.fontSizeSmall
        color: platformStyle.colorNormalMid
        elide: Text.ElideRight
        text: qsTr("by") + " " + artist + " " + qsTr("on") + " " + date
    }
    
    Label {
        id: bodyLabel
        
        anchors {
            left: avatar.right
            right: parent.right
            top: avatar.bottom
            margins: platformStyle.paddingLarge
        }
        wrapMode: Text.Wrap
        text: '"' + body + '"'
    }
    
    MouseArea {
        id: mouseArea
        
        anchors.fill: parent
        z: -1
        enabled: root.enabled
        onClicked: root.clicked()
        onPressAndHold: root.pressAndHold()
    }
}
