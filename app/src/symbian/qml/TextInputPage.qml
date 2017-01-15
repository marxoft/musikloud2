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
    
    property alias label: label.text
    property string text
    property bool multiLine: false
    property int inputMethodHints
    
    title: qsTr("Enter text")
    acceptable: text != ""
    
    KeyNavFlickable {
        id: flickable
        
        anchors.fill: parent
        contentHeight: inputContext.visible ? height : column.height + platformStyle.paddingLarge
        
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
                id: label
                
                width: parent.width
                elide: Text.ElideRight
                text: qsTr("Text")
            }
            
            Loader {
                id: loader
                
                width: parent.width
                sourceComponent: root.multiLine ? textArea : textField
            }            
        }
    }
    
    ScrollDecorator {
        flickableItem: flickable
    }
    
    Component {
        id: textField
        
        MyTextField {                
            width: parent.width
            inputMethodHints: root.inputMethodHints
            onAccepted: root.accept()
            onTextChanged: root.text = text
            Component.onCompleted: text = root.text
        }
    }
    
    Component {
        id: textArea
        
        TextArea {                
            width: parent.width
            inputMethodHints: root.inputMethodHints
            onTextChanged: root.text = text
            Component.onCompleted: text = root.text
        }
    }
}
