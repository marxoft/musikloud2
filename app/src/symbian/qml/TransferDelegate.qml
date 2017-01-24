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

    height: flow.height + platformStyle.paddingLarge * 2

    Flow {
        id: flow

        anchors {
            left: root.paddingItem.left
            right: root.paddingItem.right
            top: root.paddingItem.top
        }

        MyListItemText {
            id: titleLabel

            width: parent.width - icon.width
            mode: root.mode
            role: "Title"
            text: title
        }

        Image {
            id: icon

            width: height
            height: titleLabel.height
            source: transferType === Transfer.Upload ? "images/upload.svg" : "images/download.svg"
            smooth: true
        }
        
        MyListItemText {
            id: statusLabel

            width: parent.width
            mode: root.mode
            role: "SubTitle"
            text: statusString
        }
        
        ProgressBar {
            id: progressBar

            width: parent.width
            minimumValue: 0
            maximumValue: 100
            value: progress
        }
        
        MyListItemText {
            id: progressLabel
            
            width: parent.width
            mode: root.mode
            role: "SubTitle"
            text: progressString
        }
    }
}
