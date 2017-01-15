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

ToolButton {
    id: root

    function reset() {
        width = 240;
    }

    anchors.centerIn: parent
    flat: false
    iconSource: "images/" + (player.playing ? "play" : player.paused ? "pause" : "stop") + "-active.png"
    text: player.metaData.title
    visible: player.queueCount > 0
    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("NowPlayingPage.qml"))
    onTextChanged: reset()
    onVisibleChanged: if (visible) reset();
    onWidthChanged: if (width != 240) reset();
    Component.onCompleted: reset()
}
