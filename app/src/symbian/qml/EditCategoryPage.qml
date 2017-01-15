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

EditPage {
    id: root

    property alias name: nameEdit.text
    property alias path: pathSelector.subTitle

    title: qsTr("Category")

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
                margins: platformStyle.paddingLarge
            }
            spacing: platformStyle.paddingLarge

            Label {
                width: parent.width
                text: qsTr("Name")
            }

            MyTextField {
                id: nameEdit

                width: parent.width
                onAccepted: closeSoftwareInputPanel()
            }

            ValueListItem {
                id: pathSelector

                x: -platformStyle.paddingLarge
                width: parent.width + platformStyle.paddingLarge * 2
                title: qsTr("Download path")
                subTitle: settings.downloadPath
                onClicked: {
                    var page = appWindow.pageStack.push(Qt.resolvedUrl("FileBrowserPage.qml"),
                    {title: title, startFolder: path});
                    page.accepted.connect(function () { page.pageStack.pop(); path = page.chosenFile; });
                }
            }
        }
    }

    ScrollDecorator {
        flickableItem: flickable
    }
    
    states: State {
        name: "inputContextVisible"
        when: inputContext.visible
        
        PropertyChanges {
            target: flickable
            contentHeight: flickable.height
        }
        
        PropertyChanges {
            target: pathSelector
            visible: false
        }
    }
    
    onAccepted: {
        settings.addCategory(name, path);
        appWindow.pageStack.pop();
    }
}
