/*
 * Copyright (C) 2017 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1
import com.nokia.symbian 1.1

MyListItem {
    id: root
    
    MyListItemText {
        id: label
        
        anchors {
            left: root.paddingItem.left
            right: loader.item ? loader.left : root.paddingItem.right
            rightMargin: loader.item ? platformStyle.paddingLarge : 0
            verticalCenter: root.paddingItem.verticalCenter
        }
        verticalAlignment: Text.AlignVCenter
        role: "Title"
        mode: root.mode
        text: username
    }

    Loader {
        id: loader

        anchors {
            right: root.paddingItem.right
            verticalCenter: root.paddingItem.verticalCenter
        }
        sourceComponent: active ? checkbox : undefined
    }

    Component {
        id: checkbox

        CheckBox {
            checked: true

            MouseArea {
                z: 1
                anchors.fill: parent
            }
        }
    }
}
