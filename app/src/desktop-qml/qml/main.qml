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
import QtQml 2.2
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import MusiKloud 2.0

ApplicationWindow {
    id: window
    
    property StackView pageStack: null
    
    function search(service, query, type, order) {
        raise();
        setService(service);
        return pageStack.currentItem.search(query, type, order);
    }
    
    function setService(service) {
        if (!service) {
            return;
        }
        
        tabView.currentIndex = 2;
        
        if ((service != Settings.currentService) || (pageStack.depth == 0)) {
            Settings.currentService = service;
            pageStack.clear();
        
            if (service == Resources.SOUNDCLOUD) {
                pageStack.push({item: Qt.resolvedUrl("soundcloud/SoundCloudPage.qml"), immediate: true});
            }
            else {
                pageStack.push({item: Qt.resolvedUrl("plugins/PluginPage.qml"), properties: {service: service}, immediate: true});
            }
        }
    }    
    
    function showResource(resource) {
        if (!resource.service) {
            return false;
        }
        
        raise();
        setService(resource.service);
        return pageStack.currentItem.showResource(resource);
    }
    
    function showResourceFromUrl(url) {
        return showResource(Resources.getResourceFromUrl(url));
    }
    
    minimumWidth: 800
    minimumHeight: 500
    visible: true
    title: (player.stopped ? "" : (player.currentIndex + 1) + ". " +  player.currentTrack.artist + " - "
                                  + player.currentTrack.title + " (" + player.durationString + ") - ") + "MusiKloud2"
    menuBar: MenuBar {
        
        Menu {
            id: fileMenu
            
            title: qsTr("&File")
            
            MenuItem {
                action: Action {
                    id: urlAction
                    
                    text: qsTr("Open &URL")
                    iconName: "applications-internet"
                    shortcut: "Ctrl+U"
                    onTriggered: {
                        loader.sourceComponent = urlDialog;
                        loader.item.open();
                    }
                }
            }
            
            MenuItem {
                action: Action {
                    id: openFilesAction
                    
                    text: qsTr("&Open Files")
                    iconName: "document-open"
                    shortcut: "Ctrl+O"
                    onTriggered: {
                        loader.sourceComponent = fileDialog;
                        loader.item.mode = "play";
                        loader.item.open();
                    }
                }
            }
            
            MenuItem {
                action: Action {
                    id: addFilesAction
                    
                    text: qsTr("&Add Files")
                    iconName: "list-add"
                    shortcut: "Ctrl+A"
                    onTriggered: {
                        loader.sourceComponent = fileDialog;
                        loader.item.mode = "add";
                        loader.item.open();
                    }
                }
            }
            
            MenuItem {
                action: Action {
                    id: quitAction
                    
                    text: qsTr("Quit")
                    iconName: "application-exit"
                    shortcut: "Ctrl+Q"
                    onTriggered: Qt.quit()
                }
            }
        }
        
        Menu {
            id: playbackMenu
            
            title: qsTr("&Playback")
            
            MenuItem {
                action: Action {
                    id: playAction
                    
                    text: qsTr("Play")
                    iconName: "media-playback-start"
                    shortcut: "Ctrl+Return"
                    enabled: player.queueCount > 0
                    onTriggered: player.play()
                }
            }
            
            MenuItem {
                action: Action {
                    id: pauseAction
                    
                    text: qsTr("Pause")
                    iconName: "media-playback-pause"
                    shortcut: "Ctrl+,"
                    enabled: player.queueCount > 0
                    onTriggered: player.pause()
                }
            }
            
            MenuItem {
                action: Action {
                    id: stopAction
                    
                    text: qsTr("&Stop")
                    iconName: "media-playback-stop"
                    shortcut: "Ctrl+."
                    enabled: player.queueCount > 0
                    onTriggered: player.stop()
                }
            }
            
            MenuItem {
                action: Action {
                    id: previousAction
                    
                    text: qsTr("&Previous")
                    iconName: "media-skip-backward"
                    shortcut: "Alt+Up"
                    enabled: player.queueCount > 0
                    onTriggered: player.previous()
                }
            }
            
            MenuItem {
                action: Action {
                    id: nextAction
                    
                    text: qsTr("&Next")
                    iconName: "media-skip-forward"
                    shortcut: "Alt+Down"
                    enabled: player.queueCount > 0
                    onTriggered: player.next()
                }
            }
            
            MenuSeparator {}
            
            MenuItem {
                action: Action {
                    id: repeatAction
                    
                    text: qsTr("&Repeat")
                    shortcut: "Ctrl+R"
                    checkable: true
                    onTriggered: player.repeat = checked
                }
            }
            
            MenuItem {
                action: Action {
                    id: shuffleAction
                    
                    text: qsTr("&Shuffle")
                    shortcut: "Ctrl+S"
                    checkable: true
                    onTriggered: player.shuffle = checked
                }
            }
            
            MenuItem {
                action: Action {
                    id: stopAfterCurrentAction
                    
                    text: qsTr("Stop after current track")
                    shortcut: "Ctrl+M"
                    checkable: true
                    onTriggered: player.stopAfterCurrentTrack = checked
                }
            }
            
            MenuSeparator {}
            
            MenuItem {
                action: Action {
                    id: clearAction
                    
                    text: qsTr("&Clear playback queue")
                    iconName: "edit-clear"
                    shortcut: "Ctrl+Backspace"
                    onTriggered: player.clearQueue()
                }
            }
        }
        
        Menu {
            id: viewMenu
            
            title: qsTr("&View")
            
            ExclusiveGroup {
                id: viewGroup
            }
            
            MenuItem {
                action: Action {
                    id: queueAction
                    
                    text: qsTr("Playback queue")
                    shortcut: "Ctrl+1"
                    checkable: true
                    checked: true
                    exclusiveGroup: viewGroup
                    onTriggered: tabView.currentIndex = 0
                }
            }
                        
            MenuItem {
                action: Action {
                    id: transfersAction
                    
                    text: qsTr("Transfers")
                    shortcut: "Ctrl+2"
                    checkable: true
                    exclusiveGroup: viewGroup
                    onTriggered: tabView.currentIndex = 1
                }
            }
            
            MenuSeparator {}
            
            Instantiator {
                model: ServiceModel {}
                delegate: MenuItem {
                    action: Action {                    
                        text: name
                        shortcut: "Ctrl+" + (index + 3)
                        checkable: true
                        exclusiveGroup: viewGroup
                        onTriggered: window.setService(value)
                    }
                }
                onObjectAdded: viewMenu.insertItem(index, object)
                onObjectRemoved: viewMenu.removeItem(object)
            }
        }
        
        Menu {
            id: editMenu
            
            title: qsTr("&Edit")
            
            MenuItem {
                text: qsTr("&Preferences")
                iconName: "preferences-desktop"
                shortcut: "Ctrl+P"
                onTriggered: {
                    loader.sourceComponent = settingsDialog;
                    loader.item.open();
                }
            }
        }
        
        Menu {
            id: helpMenu
            
            title: qsTr("&Help")
            
            MenuItem {
                text: qsTr("&About")
                iconName: "help-about"
                onTriggered: {
                    loader.sourceComponent = aboutDialog;
                    loader.item.open();
                }
            }
        }
    }
    
    toolBar: ToolBar {
        
        RowLayout {
            anchors.fill: parent
            
            ToolButton {                
                action: openFilesAction
            }
            
            ToolButton {                
                action: addFilesAction
            }
            
            ToolButton {                
                action: player.playing ? pauseAction : playAction
            }
            
            ToolButton {                
                action: stopAction
            }
            
            ToolButton {
                action: previousAction
            }
            
            ToolButton {
                action: nextAction
            }
            
            Slider {                
                Layout.fillWidth: true
                maximumValue: player.duration
                value: player.position
                updateValueWhileDragging: false
                enabled: player.seekable
                opacity: player.queueCount > 0 ? 1 : 0
                onPressedChanged: if (!pressed) player.position = value;
            }
            
            Label {                
                font.bold: true
                text: player.positionString + " / " + player.durationString
                opacity: player.queueCount > 0 ? 1 : 0
            }
            
            ToolButton {
                action: repeatAction
                iconName: "media-playlist-repeat"
            }
            
            ToolButton {
                action: shuffleAction
                iconName: "media-playlist-shuffle"
            }
        }
    }           
    
    statusBar: StatusBar {
        id: statusBar
        
        function showMessage(message) {
            statusLabel.text = message;
            statusTimer.restart();
        }
        
        Timer {
            id: statusTimer
            
            interval: 3000
            onTriggered: statusLabel.text = ""
        }
        
        RowLayout {
            
            Label {
                id: statusLabel
                
                Layout.fillWidth: true
            }
        }
    }
    
    TabView {
        id: tabView
        
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: nowPlaying.top
        }
        frameVisible: false
        tabsVisible: false
        
        Tab {
            PlaybackQueuePage {
                id: queuePage
            }
        }
        
        Tab {
            TransfersPage {
                id: transfersPage
            }
        }
    
        Tab {
            StackView {
                id: stack
                
                Component.onCompleted: window.pageStack = stack
            }
        }        
    }
    
    Rectangle {
        id: nowPlaying
        
        height: 80
        anchors {
            left: parent.left
            leftMargin: 5
            right: parent.right
            rightMargin: 5
            bottom: parent.bottom
        }
        color: "#0b0b0b"
        
        Image {
            id: thumbnail
            
            width: height
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
                margins: 5
            }
            source: player.currentTrack ? player.currentTrack.thumbnailUrl : ""
        }
        
        ColumnLayout {
            id: column
            
            anchors {
                left: thumbnail.right
                right: parent.right
                top: parent.top
                bottom: parent.bottom
                margins: 5
            }
            
            Label {
                id: titleLabel
                
                Layout.fillWidth: true
                font.pixelSize: 24
                color: palette.light
                elide: Text.ElideRight
                text: player.currentTrack ? player.currentTrack.title : ""
            }
            
            Label {
                id: artistLabel
                
                Layout.fillWidth: true
                color: palette.light
                elide: Text.ElideRight
                text: player.currentTrack ? player.currentTrack.artist : ""
            }
            
            Label {
                id: genreLabel
                
                Layout.fillWidth: true
                color: palette.light
                elide: Text.ElideRight
                text: player.currentTrack ? player.currentTrack.genre : ""
            }
        }
    }
    
    MessageBox {
        id: messageBox
    }
    
    SystemPalette {
        id: palette
    }
    
    AudioPlayer {
        id: player
        
        onStatusChanged: if (status == AudioPlayer.Failed) messageBox.showError(errorString);
    }
        
    Loader {
        id: loader
    }
    
    Component {
        id: urlDialog
        
        UrlDialog {
            onAccepted: showResourceFromUrl(url)
        }
    }
    
    Component {
        id: fileDialog
        
        FileDialog {
            property string mode: "play"
            
            title: mode == "add" ? qsTr("Add audio files") : qsTr("Play audio files")
            selectExisting: true
            selectMultiple: true
            onAccepted: mode == "add" ? player.addUrls(fileUrls) : player.playUrls(fileUrls)
        }
    }
    
    Component {
        id: settingsDialog
        
        SettingsDialog {}
    }
    
    Component {
        id: aboutDialog
        
        AboutDialog {}
    }
    
    Connections {
        id: clipboardConnections
        
        target: Clipboard
        onTextChanged: showResourceFromUrl(text)
    }
    
    Connections {
        id: dbusConnections
        
        target: DBus
        onResourceRequested: showResource(resource)
    }
    
    Connections {
        id: transferConnections
        
        target: null
        onTransferAdded: statusBar.showMessage("'" + transfer.title + "' " + qsTr("added to transfers"))
    }
    
    Component.onCompleted: {
        Transfers.restoreTransfers();
        transferConnections.target = Transfers;
        
        if (DBus.requestedResource.service) {
            showResource(DBus.requestedResource);
        }
    }
}
