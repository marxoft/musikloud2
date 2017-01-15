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
    
    title: qsTr("Playback queue")
    tools: ToolBarLayout {
        BackToolButton {}
        
        MyToolButton {
            iconSource: "toolbar-view-menu"
            toolTip: qsTr("Options")
            onClicked: popups.open(menu, root)
        }
    }
    
    MyListView {
        id: view

        anchors.fill: parent
        model: player.queue
        delegate: PlaybackQueueDelegate {
            onActivated: player.currentIndex === index ? player.play() : player.setCurrentIndex(index, true)
            onClicked: player.currentIndex === index ? player.play() : player.setCurrentIndex(index, true)
            onPressAndHold: popups.open(contextMenu, root)
        }
    }
    
    ScrollDecorator {
        flickableItem: view
    }
    
    Label {
        id: label
        
        anchors {
            fill: parent
            margins: platformStyle.paddingLarge
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        color: platformStyle.colorNormalMid
        font.bold: true
        font.pixelSize: 32
        text: qsTr("No tracks")
        visible: player.queueCount === 0
    }
    
    PopupLoader {
        id: popups
    }
    
    Component {
        id: menu
        
        MyMenu {
            focusItem: view
            
            MenuLayout {
                MenuItem {
                    text: qsTr("Clear")
                    onClicked: player.clearQueue()
                }
            }
        }
    }
    
    Component {
        id: contextMenu
        
        MyContextMenu {
            focusItem: view
            
            MenuLayout {
                MenuItem {
                    text: qsTr("Play")
                    onClicked: player.currentIndex === view.currentIndex ? player.play()
                                                                         : player.setCurrentIndex(view.currentIndex, true)
                }

                MenuItem {
                    text: qsTr("Download")
                    enabled: player.queue.data(view.currentIndex, "downloadable") === true
                    onClicked: {
                        switch (player.queue.data(view.currentIndex, "service")) {
                        case mkresources.SOUNDCLOUD:
                            appWindow.pageStack.push(Qt.resolvedUrl("soundcloud/SoundCloudDownloadPage.qml"))
                            .get(player.queue.itemData(view.currentIndex));
                            break;
                        default:
                            appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginDownloadPage.qml"))
                            .list(player.queue.itemData(view.currentIndex));
                            break;
                        }
                    }
                }

                MenuItem {
                    text: qsTr("Copy URL")
                    onClicked: {
                        clipboard.text = player.queue.data(view.currentIndex, "url");
                        infoBanner.information(qsTr("URL copied to clipboard"));
                    }
                }

                MenuItem {
                    text: qsTr("Remove")
                    onClicked: player.removeTrack(view.currentIndex)
                }
            }
        }
    }

    Component.onCompleted: view.positionViewAtIndex(player.currentIndex, ListView.Beginning)
}
