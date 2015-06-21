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

ItemDelegate {
    id: root
                
    Label {
        anchors {
            left: loader.item ? loader.right : parent.left
            right: parent.right
            margins: 2
        }
        verticalAlignment: Text.AlignVCenter
        color: styleData.textColor
        elide: styleData.elideMode
        text: styleData.value        
    }
    
    Loader {
        id: loader
        
        width: height
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: 2
        }
        sourceComponent: styleData.column == 0 ? thumbnail : undefined
    }
    
    Component {
        id: thumbnail
        
        Image {
            anchors.fill: parent
            source: view.model.data(styleData.row, "thumbnailUrl")
            smooth: true
        }
    }
}
