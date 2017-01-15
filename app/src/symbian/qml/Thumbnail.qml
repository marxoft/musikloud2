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

Item {
    id: root

    property url source
    property url placeholderSource
    property alias sourceSize: thumbnail.sourceSize
    property alias asynchronous: thumbnail.asynchronous
    property alias fillMode: thumbnail.fillMode
    property alias smooth: thumbnail.smooth
    property alias status: thumbnail.status
    property bool swipeEnabled: false

    signal clicked
    signal leftSwipe
    signal rightSwipe

    Image {
        id: thumbnail

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

        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        asynchronous: true
        sourceSize.width: width
        sourceSize.height: height
        smooth: true
        onStatusChanged: if ((status == Image.Error) && (source == root.source)) source = root.placeholderSource;
    }

    MouseArea {
        id: mouseArea

        property int xPos
        property int swipeLength: Math.floor(width / 4)

        anchors.fill: parent
        enabled: root.enabled
        onPressed: xPos = mouseX
        onReleased: {
            if (containsMouse) {
                if (root.swipeEnabled) {
                    if ((mouseX - xPos) > swipeLength) {
                        root.rightSwipe();
                    }
                    else if ((xPos - mouseX) > swipeLength) {
                        root.leftSwipe();
                    }
                    else {
                        root.clicked();
                    }
                }
                else {
                    root.clicked();
                }
            }
        }
    }

    onSourceChanged: thumbnail.update()
    onPlaceholderSourceChanged: thumbnail.update()
    Component.onCompleted: {
        thumbnail.complete = true;
        thumbnail.update();
    }
}
