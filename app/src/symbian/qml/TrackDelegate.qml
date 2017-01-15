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

MyListItem {
    id: root
    
    Thumbnail {
        id: thumbnail
        
        anchors {
            left: root.paddingItem.left
            top: root.paddingItem.top
            bottom: root.paddingItem.bottom
        }
        z: 10
        width: height
        source: thumbnailUrl
        placeholderSource: "images/track.jpg"
        onClicked: root.activated()
    }
    
    MyListItemText {
        id: titleLabel
        
        anchors {
            left: thumbnail.right
            leftMargin: platformStyle.paddingLarge
            right: root.paddingItem.right
            top: root.paddingItem.top
        }
        role: "Title"
        mode: root.mode
        elide: Text.ElideRight
        text: title
    }
    
    MyListItemText {
        id: artistLabel
        
        anchors {
            left: titleLabel.left
            right: titleLabel.right
            bottom: root.paddingItem.bottom
        }
        role: "SubTitle"
        mode: root.mode
        elide: Text.ElideRight
        text: artist ? artist : qsTr("Unknown artist")
    }
}
