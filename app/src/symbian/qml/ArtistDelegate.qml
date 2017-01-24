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

MyListItem {
    id: root
    
    Avatar {
        id: avatar
        
        anchors {
            left: root.paddingItem.left
            top: root.paddingItem.top
            bottom: root.paddingItem.bottom
        }
        width: height
        source: thumbnailUrl
        placeholderSource: "images/artist.svg"
        enabled: false
    }
    
    MyListItemText {
        id: nameLabel
        
        anchors {
            left: avatar.right
            leftMargin: platformStyle.paddingLarge
            right: root.paddingItem.right
            verticalCenter: root.paddingItem.verticalCenter
        }
        role: "Title"
        mode: root.mode
        elide: Text.ElideRight
        text: name
    }
}
