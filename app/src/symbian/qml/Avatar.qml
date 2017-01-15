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
import MusiKloud 2.0

MaskedItem {
    id: root

    property url source
    property url placeholderSource
    property alias sourceSize: avatar.sourceSize
    property alias asynchronous: avatar.asynchronous
    property alias fillMode: avatar.fillMode
    property alias smooth: avatar.smooth
    property alias status: avatar.status

    signal clicked
    signal pressAndHold
    
    mask: Image {
        width: root.width
        height: root.height
        source: "images/avatar-mask.png"
        fillMode: Image.Stretch
        smooth: true
    }

    Image {
        id: frame

        anchors.fill: parent
        source: "images/avatar-frame.png"
        sourceSize.width: width
        sourceSize.height: height
        smooth: avatar.smooth
        fillMode: Image.Stretch
        visible: avatar.status == Image.Ready

        Image {
            id: avatar

            property bool complete: false

            function update() {
                if (complete) {
                    if (!root.source.toString()) {
                        source = root.placeholderSource;
                    }
                    else {
                        source = root.source;
                    }
                }
            }

            z: -1
            anchors.fill: parent
            sourceSize.width: width
            sourceSize.height: height
            smooth: true
            fillMode: Image.PreserveAspectCrop
            clip: true
            asynchronous: true
            onStatusChanged: if ((status == Image.Error) && (source == root.source)) source = root.placeholderSource;
        }
    }
    
    MouseArea {
        id: mouseArea

        anchors.fill: parent
        enabled: root.enabled
        onClicked: root.clicked()
        onPressAndHold: root.pressAndHold()
    }

    onSourceChanged: avatar.update()
    onPlaceholderSourceChanged: avatar.update()
    Component.onCompleted: {
        avatar.complete = true;
        avatar.update();
    }
}
