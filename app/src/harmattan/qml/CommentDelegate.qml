/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

ListItem {
    id: root

    signal thumbnailClicked

    height: userLabel.height + commentLabel.height + UI.PADDING_DOUBLE * 2

    Avatar {
        id: avatar

        z: 100
        width: 48
        height: 48
        anchors {
            left: parent.left
            top: parent.top
            margins: UI.PADDING_DOUBLE
        }
        source: thumbnailUrl ? thumbnailUrl : "images/avatar.png"
        onClicked: root.thumbnailClicked()
    }

    Label {
        id: userLabel

        anchors {
            left: avatar.right
            right: parent.right
            top: parent.top
            margins: UI.PADDING_DOUBLE
        }
        font.pixelSize: UI.FONT_SMALL
        font.family: UI.FONT_FAMILY_LIGHT
        text: qsTr("by") + " " + artist + " " + qsTr("on") + " " + date
    }

    Label {
        id: commentLabel

        anchors {
            left: userLabel.left
            right: parent.right
            top: userLabel.bottom
            rightMargin: UI.PADDING_DOUBLE
        }
        font.pixelSize: UI.FONT_SMALL
        text: "\"" + body + "\""
    }
}
