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

Item {
    id: root
    
    signal pressed
    signal clicked
    signal doubleClicked
    signal rightClicked
    
    MouseArea {
        id: mouseArea
        
        z: 1000
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: root.pressed()
        onClicked: mouse.button == Qt.RightButton ? root.rightClicked() : root.clicked()
        onDoubleClicked: if (mouse.button == Qt.LeftButton) root.doubleClicked();
    }
}
