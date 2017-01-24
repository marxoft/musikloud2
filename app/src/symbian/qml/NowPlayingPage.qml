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

MyPage {
    id: root

    property bool playWhenActive: false
    
    title: qsTr("Now playing")
    tools: ToolBarLayout {
        BackToolButton {}
        
        MyToolButton {
            iconSource: "images/repeat.svg"
            toolTip: qsTr("Repeat")
            checkable: true
            checked: player.repeat
            onClicked: player.repeat = !player.repeat
        }
        
        MyToolButton {
            iconSource: "images/shuffle.svg"
            toolTip: qsTr("Shuffle")
            checkable: true
            checked: player.shuffle
            onClicked: player.shuffle = !player.shuffle
        }
        
        MyToolButton {
            iconSource: "toolbar-view-menu"
            toolTip: qsTr("Options")
            onClicked: popups.open(menu, root)
        }
    }
    
    Thumbnail {
        id: thumbnail
        
        anchors {
            left: parent.left
            top: parent.top
            margins: platformStyle.paddingLarge * 2
        }
        width: parent.width - platformStyle.paddingLarge * 4
        height: width
        source: player.metaData.largeThumbnailUrl
        placeholderSource: "images/music.svg"
        sourceSize.width: 360
        sourceSize.height: 360
        swipeEnabled: true
        opacity: status == Image.Ready ? 1 : 0
        onLeftSwipe: player.next()
        onRightSwipe: player.previous()
    }
    
    Item {
        id: progressContainer
        
        anchors {
            left: thumbnail.left
            right: parent.right
            rightMargin: platformStyle.paddingLarge * 2
            topMargin: platformStyle.paddingLarge
            bottom: thumbnail.bottom
        }
        height: progressSlider.height + positionLabel.height + platformStyle.paddingLarge
        
        Rectangle {
            id: progressBackground

            anchors.fill: parent
            color: "#000"
            opacity: 0.7
        }
    
        Slider {
            id: progressSlider
            
            anchors {
                left: parent.left
                leftMargin: platformStyle.paddingSmall
                right: parent.right
                rightMargin: platformStyle.paddingSmall
                top: parent.top
            }
            minimumValue: 0
            maximumValue: Math.max(1, player.duration)
            enabled: player.seekable
            onEnabledChanged: if (!enabled) value = 0;
            onPressedChanged: if (!pressed) player.position = value;
            onValueChanged: positionLabel.text = utils.formatMSecs(value)
        }
        
        Label {
            id: positionLabel
            
            anchors {
                left: parent.left
                leftMargin: platformStyle.paddingLarge
                top: progressSlider.bottom
            }
            font.pixelSize: platformStyle.fontSizeSmall
            text: "--:--"
        }

        Label {
            id: bufferLabel

            anchors {
                left: positionLabel.right
                leftMargin: platformStyle.paddingLarge
                right: durationLabel.left
                rightMargin: platformStyle.paddingLarge
                top: progressSlider.bottom
            }
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: platformStyle.fontSizeSmall
            color: platformStyle.colorNormalMid
            elide: Text.ElideRight
        }
        
        Label {
            id: durationLabel
            
            anchors {
                right: parent.right
                rightMargin: platformStyle.paddingLarge
                top: progressSlider.bottom
            }
            font.pixelSize: platformStyle.fontSizeSmall
            text: player.metaData.durationString ? player.metaData.durationString : "--:--"
        }
    }
    
    Column {
        id: infoColumn
        
        anchors {
            left: thumbnail.left
            right: parent.right
            rightMargin: platformStyle.paddingLarge * 2
            top: thumbnail.bottom
            topMargin: platformStyle.paddingLarge
        }

        Label {
            id: countLabel

            width: parent.width
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
            color: platformStyle.colorNormalMid
            font.pixelSize: platformStyle.fontSizeSmall
            text: player.queueCount > 0 ? (player.currentIndex + 1) + "/" + player.queueCount + " " + qsTr("tracks")
                                        : qsTr("No tracks")
        }
            
        Label {
            id: artistLabel
            
            width: parent.width
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
            text: player.metaData.artist ? player.metaData.artist : qsTr("Unknown artist")
        }
        
        Label {
            id: titleLabel
            
            width: parent.width
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
            text: player.metaData.title ? player.metaData.title : qsTr("Unknown title")
        }
        
        Label {
            id: genreLabel
            
            width: parent.width
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: platformStyle.fontSizeSmall
            text: player.metaData.genre ? player.metaData.genre : qsTr("Unknown genre")
        }
    }
    
    ButtonRow {
        id: buttonRow
        
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: platformStyle.paddingLarge * 2
        }
        enabled: player.queueCount > 0
        exclusive: false
        
        Button {
            id: previousButton
            
            iconSource: "images/previous.svg"
            onClicked: player.previous()
        }
        
        Button {
            id: playPauseButton
            
            iconSource: privateStyle.imagePath("toolbar-mediacontrol-"
            + ((player.paused) || (player.stopped) ? "play" : player.seekable ? "pause" : "stop"))
            onClicked: player.playing = !player.playing
        }
        
        Button {
            id: nextButton
            
            iconSource: "images/next.svg"
            onClicked: player.next()
        }
    }
    
    PopupLoader {
        id: popups
    }
    
    Component {
        id: menu
        
        MyMenu {
            focusItem: buttonRow
            
            MenuLayout {
                MenuItem {
                    text: qsTr("Track details")
                    enabled: player.queueCount > 0
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("NowPlayingTrackPage.qml"))
                }

                MenuItem {
                    text: qsTr("Playback queue")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PlaybackQueuePage.qml"))
                }

                MenuItem {
                    text: qsTr("Playback settings")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PlaybackSettingsPage.qml"))
                }
            }
        }
    }
    
    Connections {
        id: playerConnections

        target: null
        onBufferStatusChanged: bufferLabel.text = qsTr("Buffering") + " " + player.bufferStatus + "%"
        onPositionChanged: {
            if (progressSlider.enabled) {
                if (!progressSlider.pressed) {
                    progressSlider.value = player.position;
                }
            }
            else {
                positionLabel.text = player.positionString;
            }
        }
        onStatusChanged: {
            switch (player.status) {
            case AudioPlayer.Loading:
                bufferLabel.text = qsTr("Loading")
                break;
            case AudioPlayer.Buffering:
                bufferLabel.text = qsTr("Buffering") + " " + player.bufferStatus + "%";
                break;
            default:
                bufferLabel.text = "";
                break;
            }
        }
    }

    Connections {
        id: symbianConnections

        target: symbian
        onForegroundChanged: {
            if (symbian.foreground) {
                if (progressSlider.enabled) {
                    if (!progressSlider.pressed) {
                        progressSlider.value = player.position;
                    }
                }
                else {
                    positionLabel.text = player.positionString;
                }

                switch (player.status) {
                case AudioPlayer.Loading:
                    bufferLabel.text = qsTr("Loading")
                    break;
                case AudioPlayer.Buffering:
                    bufferLabel.text = qsTr("Buffering") + " " + player.bufferStatus + "%";
                    break;
                default:
                    bufferLabel.text = "";
                    break;
                }

                playerConnections.target = player;
            }
            else {
                playerConnections.target = null;
            }
        }
    }
    
    states: State {
        name: "landscape"
        when: !appWindow.inPortrait
        
        PropertyChanges {
            target: thumbnail
            anchors.margins: platformStyle.paddingLarge
        }
        
        PropertyChanges {
            target: thumbnail
            width: parent.height - platformStyle.paddingLarge * 2
        }
        
        AnchorChanges {
            target: progressContainer
            anchors {
                left: thumbnail.right
                top: infoColumn.bottom
                bottom: undefined
            }
        }

        PropertyChanges {
            target: progressContainer
            anchors {
                leftMargin: platformStyle.paddingLarge
                rightMargin: platformStyle.paddingLarge
            }
        }

        PropertyChanges {
            target: progressBackground
            opacity: 0
        }
        
        AnchorChanges {
            target: infoColumn
            anchors {
                left: thumbnail.right
                top: parent.top
            }
        }

        PropertyChanges {
            target: infoColumn
            anchors.leftMargin: platformStyle.paddingLarge * 2
        }

        AnchorChanges {
            target: buttonRow
            anchors.left: thumbnail.right
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Active) {
            if (playWhenActive) {
                player.play();
            }

            playWhenActive = false;

            if (progressSlider.enabled) {
                if (!progressSlider.pressed) {
                    progressSlider.value = player.position;
                }
            }
            else {
                positionLabel.text = player.positionString;
            }

            switch (player.status) {
            case AudioPlayer.Loading:
                bufferLabel.text = qsTr("Loading")
                break;
            case AudioPlayer.Buffering:
                bufferLabel.text = qsTr("Buffering") + " " + player.bufferStatus + "%";
                break;
            default:
                bufferLabel.text = "";
                break;
            }

            playerConnections.target = player;
        }
        else {
            playerConnections.target = null;
        }
    }
}
