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
import Qt.labs.folderlistmodel 1.0
import com.nokia.symbian 1.1

MyPage {
    id: root
    
    property bool showFiles: false
    property string startFolder: "E:/"
    property string chosenFile
    property string chosenUrl
    
    signal accepted
    signal rejected
    
    title: qsTr("Choose folder")
    tools: ToolBarLayout {
        BackToolButton {
            onClicked: {
                root.chosenFile = "";
                root.chosenUrl = "";
                root.rejected();
            }
        }
        
        MyToolButton {
            iconSource: "images/up.png"
            toolTip: qsTr("Go up")
            onClicked: folderListModel.folder = folderListModel.parentFolder
        }
        
        MyToolButton {
            iconSource: "images/yes.png"
            toolTip: qsTr("Submit")
            enabled: folderText.text !== ""
            onClicked: {
                if (root.showFiles) {
                    root.chosenUrl = fileList.chosenFile;
                }
                else {
                    root.chosenUrl = folderListModel.folder.toString();
                }

                root.chosenFile = root.chosenUrl.slice(8);
                root.accepted();
            }
        }
    }
    
    Label {
        id: folderText

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: platformStyle.paddingLarge
        }
        elide: Text.ElideRight
        color: platformStyle.colorNormalLink
        text: showFiles ? fileList.chosenFile.slice(fileList.chosenFile.lastIndexOf("/") + 1)
        : folderListModel.folder.toString().slice(8)
    }

    HeaderLabel {
        id: header

        anchors {
            left: parent.left
            right: parent.right
            top: folderText.bottom
            margins: platformStyle.paddingLarge
        }

        text: showFiles ? qsTr("Files") : qsTr("Folders")
    }

    MyListView {
        id: fileList

        property string chosenFile: ""

        anchors {
            left: parent.left
            right: parent.right
            top: header.bottom
            topMargin: platformStyle.paddingLarge
            bottom: parent.bottom
        }
        clip: true
        model: FolderListModel {
            id: folderListModel

            nameFilters: root.showFiles ? [] : ["*.foo_bar"]
            folder: "file:///" + root.startFolder
            showDotAndDotDot: false
            showDirs: true
        }
        delegate: MyListItem {
            Image {
                id: icon

                anchors {
                    left: paddingItem.left
                    verticalCenter: paddingItem.verticalCenter
                }

                source: "images/folder.png"
            }

            MyListItemText {
                anchors {
                    left: icon.right
                    leftMargin: platformStyle.paddingLarge
                    right: paddingItem.right
                    verticalCenter: paddingItem.verticalCenter
                }
                role: "Title"
                elide: Text.ElideRight
                text: fileName
            }
            
            onClicked: folderListModel.isFolder(index) ? folderListModel.folder = filePath
            : fileList.chosenFile = filePath
        }
    }

    ScrollDecorator {
        flickableItem: fileList
    }

    Label {
        id: noResultsText

        anchors {
            fill: parent
            margins: platformStyle.paddingLarge
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        font.pixelSize: 32
        font.bold: true
        color: platformStyle.colorNormalMid
        text: qsTr("Folder empty")
        visible: fileList.count === 0
    }
}
