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
    
    title: qsTr("General")
    tools: ToolBarLayout {
        BackToolButton {}
    }
    
    KeyNavFlickable {
        id: flickable
        
        anchors.fill: parent
        contentHeight: column.height + platformStyle.paddingLarge
        
        Column {
            id: column
            
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            ValueSelector {
                id: orientationSelector

                width: parent.width
                title: qsTr("Screen orientation")
                model: ScreenOrientationModel {}
                value: settings.screenOrientation
                onAccepted: settings.screenOrientation = value
            }
            
            ValueListItem {
                id: pathSelector
                
                width: parent.width
                title: qsTr("Download path")
                subTitle: settings.downloadPath
                onClicked: {
                    var page = appWindow.pageStack.push(Qt.resolvedUrl("FileBrowserPage.qml"),
                    {title: title, startFolder: settings.downloadPath});
                    page.accepted.connect(function () { page.pageStack.pop(); settings.downloadPath = page.chosenFile; });
                }
            }

            TextDelegate {
                id: categoriesButton

                flickableMode: true
                width: parent.width
                subItemIndicator: true
                text: qsTr("Categories")
                onClicked: appWindow.pageStack.push(Qt.resolvedUrl("CategorySettingsPage.qml"))
            }
            
            ValueSelector {
                id: concurrentSelector
                
                width: parent.width
                title: qsTr("Maximum concurrent transfers")
                model: ConcurrentTransfersModel {}
                value: settings.maximumConcurrentTransfers
                onAccepted: settings.maximumConcurrentTransfers = value
            }

            Item {                
                width: parent.width
                height: platformStyle.paddingLarge
            }
            
            MySwitch {
                id: automaticSwitch
                
                x: platformStyle.paddingLarge
                width: parent.width - platformStyle.paddingLarge * 2
                text: qsTr("Start transfers automatically")
                checked: settings.startTransfersAutomatically
                onCheckedChanged: settings.startTransfersAutomatically = checked
            }

            Item {
                width: parent.width
                height: platformStyle.paddingLarge
            }
            
            MySwitch {
                id: restoreQueueSwitch
                
                x: platformStyle.paddingLarge
                width: parent.width - platformStyle.paddingLarge * 2
                text: qsTr("Restore playback queue on startup")
                checked: settings.restorePlaybackQueueOnStartup
                onCheckedChanged: settings.restorePlaybackQueueOnStartup = checked
            }
        }
    }
    
    ScrollDecorator {
        flickableItem: flickable
    }
}
