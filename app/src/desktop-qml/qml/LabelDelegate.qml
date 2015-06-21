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
    
    property alias color: label.color
    property alias elide: label.elide
    property alias font: label.font
    property alias text: label.text
                
    Label {
        id: label
        
        anchors {
            fill: parent
            leftMargin: 2
            rightMargin: 2
        }
        color: styleData.textColor
        elide: styleData.elideMode
        text: styleData.value
    }
}
