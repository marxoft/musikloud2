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

    title: qsTr("Media/content")
    tools: ToolBarLayout {

        BackToolIcon {}
    }

    Flickable {
        id: flicker

        anchors.fill: parent
        contentHeight: column.height + UI.PADDING_DOUBLE

        Column {
            id: column

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            
            PageHeader {
                title: root.title
            }

            ValueListItem {
                width: parent.width
                title: qsTr("Default download path")
                subTitle: Settings.downloadPath
                onClicked: {
                    loader.sourceComponent = pathDialog;
                    loader.item.open();
                }
            }

            DrillDownDelegate {
                width: parent.width
                text: qsTr("Categories")
                onClicked: appWindow.pageStack.push(Qt.resolvedUrl("CategoriesPage.qml"))
            }

            Item {
                width: parent.width
                height: UI.PADDING_DOUBLE
            }

            MySwitch {
                width: parent.width
                text: qsTr("Monitor clipboard for URLs")
                checked: Settings.clipboardMonitorEnabled
                onCheckedChanged: Settings.clipboardMonitorEnabled = checked
            }
        }
    }

    ScrollDecorator {
        flickableItem: flicker
    }

    Loader {
        id: loader
    }

    Component {
        id: pathDialog

        FileBrowserDialog {
            onFileChosen: Settings.downloadPath = filePath
        }
    }
}
