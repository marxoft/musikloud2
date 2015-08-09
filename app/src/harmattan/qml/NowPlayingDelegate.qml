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

    height: thumbnail.height + UI.PADDING_DOUBLE * 2

    Thumbnail {
        id: thumbnail

        z: 100
        width: 64
        height: 64
        anchors {
            left: parent.left
            leftMargin: UI.PADDING_DOUBLE
            verticalCenter: parent.verticalCenter
        }
        source: thumbnailUrl
        placeholderText: title
        enabled: false
    }

    Flow {
        id: flow

        anchors {
            left: thumbnail.right
            leftMargin: UI.PADDING_DOUBLE
            right: parent.right
            rightMargin: UI.PADDING_DOUBLE
            verticalCenter: parent.verticalCenter
        }
        spacing: UI.PADDING_DOUBLE

        Label {
            id: titleLabel
            
            width: parent.width
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            color: player.currentIndex == index ? Settings.activeColor : UI.COLOR_INVERTED_FOREGROUND
            text: title
        }        

        Label {
            id: artistLabel
            
            width: parent.width - durationLabel.width - parent.spacing
            font.pixelSize: UI.FONT_SMALL
            font.family: UI.FONT_FAMILY_LIGHT
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            color: player.currentIndex == index ? Settings.activeColor : UI.COLOR_INVERTED_SECONDARY_FOREGROUND
            text: artist ? artist : qsTr("Unknown artist")
        }
        
        Label {
            id: durationLabel
            
            font.pixelSize: UI.FONT_SMALL
            font.family: UI.FONT_FAMILY_LIGHT
            verticalAlignment: Text.AlignVCenter
            color: player.currentIndex == index ? Settings.activeColor : UI.COLOR_INVERTED_SECONDARY_FOREGROUND
            text: durationString
        }
    }
}
