/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import MusiKloud 2.0
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    property bool showQueue: false
    property bool autoPlay: false

    tools: ToolBarLayout {

        BackToolIcon {}
        
        ToolIcon {
            iconSource: "images/shuffle" + (player.shuffle ? "-" + Settings.activeColorString : "") + ".png"
            onClicked: player.shuffle = !player.shuffle
        }

        ToolIcon {
            iconSource: "images/repeat" + (player.repeat ? "-" + Settings.activeColorString : "") + ".png"
            onClicked: player.repeat = !player.repeat
        }
    }

    Connections {
        target: player
        onQueueCountChanged: if (!player.queueCount) appWindow.pageStack.pop();
    }

    Image {
        id: leftBackground

        anchors {
            left: parent.left
            right: thumbnail.right
            top: parent.top
            bottom: parent.bottom
        }
        visible: !appWindow.inPortrait
        source: visible ? "image://theme/meegotouch-applicationpage-background-inverted" : ""
        smooth: true
        fillMode: Image.Stretch
    }

    Image {
        id: rightBackground

        anchors {
            left: flow.left
            right: flow.right
            top: parent.top
            bottom: parent.bottom
        }
        visible: !appWindow.inPortrait
        source: visible ? "image://theme/meegotouch-applicationpage-background-inverted" : ""
        smooth: true
        fillMode: Image.Stretch
    }

    Thumbnail {
        id: thumbnail

        width: appWindow.inPortrait ? 480 : parent.height - UI.PADDING_DOUBLE * 2
        height: appWindow.inPortrait ? 480 : parent.height - UI.PADDING_DOUBLE * 2
        anchors {
            top: parent.top
            topMargin: appWindow.inPortrait ? 0 : UI.PADDING_DOUBLE
            left: parent.left
            leftMargin: appWindow.inPortrait ? 0 : UI.PADDING_DOUBLE
        }
        source: player.currentTrack.largeThumbnailUrl
        placeholderText: player.currentTrack.title
        swipeEnabled: true
        onClicked: root.showQueue = true
        onLeftSwipe: player.next()
        onRightSwipe: player.previous()

        Image {
            z: -1
            anchors.fill: parent
            visible: thumbnail.text != ""
            source: visible ? "image://theme/meegotouch-applicationpage-background-inverted" : ""
            smooth: true
            fillMode: Image.Stretch
        }        
    }

    Image {
        anchors.fill: flow
        visible: appWindow.inPortrait
        source: visible ? "image://theme/meegotouch-applicationpage-background-inverted" : ""
        smooth: true
        fillMode: Image.Stretch
    }

    Flow {
        id: flow

        spacing: UI.PADDING_DOUBLE
        anchors {
            left: appWindow.inPortrait ? parent.left : thumbnail.right
            right: parent.right
            top: appWindow.inPortrait ? thumbnail.bottom : parent.top
            margins: UI.PADDING_DOUBLE
        }

        Label {
            id: titleLabel

            width: parent.width - indexLabel.width - UI.PADDING_DOUBLE
            font.bold: true
            elide: Text.ElideRight
            text: player.currentTrack.title
        }

        Label {
            id: indexLabel

            color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND
            font.family: UI.FONT_FAMILY_LIGHT
            text: (player.currentIndex + 1) + "/" + player.queueCount
        }

        Label {
            id: artistLabel

            width: parent.width
            elide: Text.ElideRight
            color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND
            font.family: UI.FONT_FAMILY_LIGHT
            text: !player.currentTrack.artist ? qsTr("Unknown artist") : player.currentTrack.artist
        }
    }

    Image {
        anchors {
            left: flow.left
            right: flow.right
            top: flow2.top
            bottom: flow2.bottom
        }
        visible: appWindow.inPortrait
        source: visible ? "image://theme/meegotouch-applicationpage-background-inverted" : ""
        smooth: true
        fillMode: Image.Stretch
    }

    Flow {
        id: flow2

        anchors {
            left: flow.left
            right: flow.right
            bottom: parent.bottom
            margins: UI.PADDING_DOUBLE
        }

        Item {
            width: parent.width
            height: 100

            MediaButton {
                id: previousButton

                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                source: "image://theme/icon-m-toolbar-mediacontrol-previous" + (pressed ? "-dimmed" : "") + "-white"
                onClicked: player.previous()
            }

            MediaButton {
                id: playButton

                anchors.centerIn: parent
                source: "image://theme/icon-m-toolbar-mediacontrol"
                        + (player.playing ? player.seekable ? "-pause" : "-stop" : "-play")
                        + (pressed ? "-dimmed" : "") + "-white"
                onClicked: player.playing = !player.playing
            }

            MediaButton {
                id: nextButton

                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                source: "image://theme/icon-m-toolbar-mediacontrol-next" + (pressed ? "-dimmed" : "") + "-white"
                onClicked: player.next()
            }
        }

        MyProgressBar {
            id: progressBar

            width: parent.width
            maximumValue: Math.max(1, player.duration)
            indeterminate: (player.status == AudioPlayer.Loading) || (player.status == AudioPlayer.Buffering)

            SeekBubble {
                id: seekBubble

                anchors.bottom: parent.top
                opacity: value != "" ? 1 : 0
                value: (seekMouseArea.drag.active) && (seekMouseArea.posInsideDragArea)
                       ? Utils.formatMSecs(Math.floor((seekMouseArea.mouseX / seekMouseArea.width) * player.duration))
                       : ""
            }

            MouseArea {
                id: seekMouseArea

                property bool posInsideMouseArea: false
                property bool posInsideDragArea: (seekMouseArea.mouseX >= 0)
                                                 && (seekMouseArea.mouseX <= seekMouseArea.drag.maximumX)

                width: parent.width
                height: 60
                anchors.centerIn: parent
                drag.target: seekBubble
                drag.axis: Drag.XAxis
                drag.minimumX: -40
                drag.maximumX: width - 10
                enabled: player.seekable
                onExited: posInsideMouseArea = false
                onEntered: posInsideMouseArea = true
                onPressed: {
                    posInsideMouseArea = true;
                    seekBubble.x = mouseX - 40;
                }
                onReleased: {
                    if (posInsideMouseArea) {
                        player.position = Math.floor((mouseX / width) * player.duration);
                    }
                }
            }

            Component.onCompleted: value = player.position
        }

        Label {
            id: positionLabel

            height: 40
            width: parent.width - durationLabel.width
            verticalAlignment: Text.AlignVCenter
            font.family: UI.FONT_FAMILY_LIGHT
            font.pixelSize: UI.FONT_SMALL
            color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND

            Component.onCompleted: text = player.positionString
        }

        Label {
            id: durationLabel

            height: 40
            verticalAlignment: Text.AlignVCenter
            color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND
            font.family: UI.FONT_FAMILY_LIGHT
            font.pixelSize: UI.FONT_SMALL
            text: player.durationString
        }
    }

    Item {
        id: queueItem

        z: -1
        visible: false
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: appWindow.inPortrait ? flow2.top : parent.bottom
        }

        Image {
            id: header

            z: 1
            height: 72
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            source: "image://theme/meegotouch-view-header-fixed-inverted"
            fillMode: Image.Stretch
            smooth: true

            MouseArea {
                anchors.fill: parent
            }

            Label {
                anchors {
                    left: parent.left
                    leftMargin: UI.PADDING_DOUBLE
                    verticalCenter: parent.verticalCenter
                }
                font.pixelSize: UI.FONT_XLARGE
                text: qsTr("Playback queue")
            }

            ToolButton {
                width: 50
                height: 50
                anchors {
                    right: parent.right
                    rightMargin: UI.PADDING_DOUBLE
                    verticalCenter: parent.verticalCenter
                }
                iconSource: "images/play-accent-" + Settings.activeColorString + ".png"
                onClicked: root.showQueue = false
            }
        }

        ListView {
            id: view

            property int selectedIndex

            anchors {
                top: header.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            onVisibleChanged: if (visible) positionViewAtIndex(player.currentIndex, ListView.Center);
            model: player.queue
            delegate: NowPlayingDelegate {
                onClicked: player.currentIndex = index
                onPressAndHold: queueMenu.open()
            }
        }

        ScrollDecorator {
            flickableItem: view
        }

        ContextMenu {
            id: queueMenu

            MenuLayout {

                MenuItem {
                    text: qsTr("Remove from playback queue")
                    onClicked: player.removeTrack(view.selectedIndex)
                }

                MenuItem {
                    text: qsTr("Clear playback queue")
                    onClicked: player.clearQueue()
                }
            }
        }
    }

    Timer {
        id: positionTimer

        interval: 10000
        repeat: true
        running: (platformWindow.viewMode === WindowState.Thumbnail) && (player.playing)
        onTriggered: {
            progressBar.value = player.position;
            positionLabel.text = player.positionString;
        }
    }

    Binding {
        target: progressBar
        when: (platformWindow.viewMode === WindowState.Fullsize) && (player.duration > 0)
        property: "value"
        value: player.position
    }

    Binding {
        target: positionLabel
        when: platformWindow.viewMode === WindowState.Fullsize
        property: "text"
        value: player.positionString
    }

    states: [
        State {
            name: "showQueue"
            when: (root.showQueue) && (appWindow.inPortrait)
            
            PropertyChanges {
                target: queueItem
                visible: true
            }
            
            AnchorChanges {
                target: thumbnail
                anchors {
                    top: undefined
                    bottom: parent.top
                    left: parent.left
                }
            }
            
            AnchorChanges {
                target: flow
                anchors {
                    top: flow2.top
                    left: parent.left
                    right: parent.right
                }
            }
        },

        State {
            name: "showQueueLandscape"
            when: (root.showQueue) && (!appWindow.inPortrait)
            
            PropertyChanges {
                target: queueItem
                visible: true
            }
            
            AnchorChanges {
                target: thumbnail
                anchors {
                    top: parent.top
                    bottom: undefined
                    left: undefined
                    right: parent.left
                }
            }
            
            AnchorChanges {
                target: flow
                anchors {
                    top: undefined
                    left: parent.right
                    right: undefined
                }
            }
        }
    ]

    transitions: [
        Transition {
            AnchorAnimation {
                duration: root.status === PageStatus.Active ? 250 : 0
            }
        }
    ]

    onStatusChanged: {
        if (status === PageStatus.Active) {
            if ((autoPlay) && (!player.paused)) {
                player.play();
            }

            autoPlay = false;
        }
    }
}
