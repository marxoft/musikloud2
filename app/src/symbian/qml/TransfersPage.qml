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

    title: qsTr("Transfers")
    tools: ToolBarLayout {

        BackToolButton {}

        MyToolButton {
            iconSource: "toolbar-mediacontrol-play"
            toolTip: qsTr("Start")
            enabled: transfers.count > 0
            onClicked: transfers.start()
        }

        MyToolButton {
            iconSource: "toolbar-mediacontrol-pause"
            toolTip: qsTr("Pause")
            enabled: transfers.count > 0
            onClicked: transfers.pause()
        }
    }

    MyListView {
        id: view

        anchors.fill: parent
        model: TransferModel {
            id: transferModel
        }
        delegate: TransferDelegate {
            onClicked: popups.open(contextMenu, root)
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
        font.pixelSize: 32
        color: platformStyle.colorNormalMid
        text: qsTr("No transfers")
        visible: transfers.count === 0
    }
    
    PopupLoader {
        id: popups
    }
    
    Component {
        id: contextMenu
        
        MyContextMenu {
            id: contextMenu

            focusItem: view

            MenuLayout {

                MenuItem {
                    text: qsTr("Start")
                    enabled: transferModel.data(view.currentIndex, "status") <= Transfer.Queued
                    onClicked: transfers.get(view.currentIndex).queue()
                }

                MenuItem {
                    text: qsTr("Pause")
                    enabled: transferModel.data(view.currentIndex, "status") >= Transfer.Downloading
                    onClicked: transfers.get(view.currentIndex).pause()
                }

                MenuItem {
                    text: qsTr("Priority")
                    platformSubItemIndicator: true
                    onClicked: popups.open(priorityDialog, root)
                }

                MenuItem {
                    text: qsTr("Category")
                    platformSubItemIndicator: true
                    onClicked: popups.open(categoryDialog, root)
                }

                MenuItem {
                    text: qsTr("Remove")
                    onClicked: popups.open(removeDialog, root)
                }
            }
        }
    }

    Component {
        id: priorityDialog

        ValueDialog {
            focusItem: view
            titleText: qsTr("Priority")
            model: TransferPriorityModel {}
            onAccepted: transfers.get(view.currentIndex).priority = value
        }
    }

    Component {
        id: categoryDialog

        ValueDialog {
            focusItem: view
            titleText: qsTr("Category")
            model: CategoryNameModel {}
            onAccepted: transfers.get(view.currentIndex).category = value
        }
    }
    
    Component {
        id: removeDialog
        
        MyQueryDialog {
            titleText: qsTr("Remove?")
            message: qsTr("Do you want to remove") + " '" + transferModel.data(view.currentIndex, "title")+ "'?"
            onAccepted: transfers.get(view.currentIndex).cancel()
        }
    }
}
