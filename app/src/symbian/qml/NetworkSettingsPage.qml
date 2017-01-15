/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
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
    
    title: qsTr("Network")
    tools: ToolBarLayout {
        BackToolButton {
            onClicked: settings.setNetworkProxy()
        }
    }
    
    KeyNavFlickable {
        id: flickable
        
        anchors.fill: parent
        contentHeight: inputContext.visible ? height : column.height + platformStyle.paddingLarge * 2
        
        Column {
            id: column
            
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: platformStyle.paddingLarge
            }
            spacing: platformStyle.paddingLarge
            
            MySwitch {
                id: proxySwitch
                
                width: parent.width
                text: qsTr("Use network proxy")
                checked: settings.networkProxyEnabled
                visible: !inputContext.visible
                onCheckedChanged: settings.networkProxyEnabled = checked
            }
            
            ValueSelector {
                id: typeSelector
                
                x: -platformStyle.paddingLarge
                width: parent.width + platformStyle.paddingLarge * 2
                focusItem: flickable
                title: qsTr("Type")
                model: NetworkProxyTypeModel {}
                value: settings.networkProxyType
                visible: !inputContext.visible
                onValueChanged: settings.networkProxyType = value
            }
            
            Label {
                width: parent.width
                elide: Text.ElideRight
                text: qsTr("Host")
                visible: hostEdit.visible
            }
            
            MyTextField {
                id: hostEdit
                
                width: parent.width
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                text: settings.networkProxyHost
                visible: (!inputContext.visible) || (activeFocus)
                onTextChanged: settings.networkProxyHost = text
                onAccepted: portEdit.forceActiveFocus()
            }
            
            Label {
                width: parent.width
                elide: Text.ElideRight
                text: qsTr("Port")
                visible: portEdit.visible
            }
            
            MyTextField {
                id: portEdit
                
                width: parent.width
                inputMethodHints: Qt.ImhDigitsOnly
                text: settings.networkProxyPort
                visible: (!inputContext.visible) || (activeFocus)
                validator: IntValidator {
                    bottom: 0
                    top: 100000
                }
                onTextChanged: settings.networkProxyPort = parseInt(text)
                onAccepted: closeSoftwareInputPanel()
            }
            
            Label {
                width: parent.width
                elide: Text.ElideRight
                text: qsTr("Username")
                visible: usernameEdit.visible
            }
            
            MyTextField {
                id: usernameEdit
                
                width: parent.width
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                text: settings.networkProxyUsername
                visible: (!inputContext.visible) || (activeFocus)
                onTextChanged: settings.networkProxyUsername = text
                onAccepted: passwordEdit.forceActiveFocus()
            }
            
            Label {
                width: parent.width
                elide: Text.ElideRight
                text: qsTr("Password")
                visible: passwordEdit.visible
            }
            
            MyTextField {
                id: passwordEdit
                
                width: parent.width
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                text: settings.networkProxyPassword
                visible: (!inputContext.visible) || (activeFocus)
                onTextChanged: settings.networkProxyPassword = text
                onAccepted: closeSoftwareInputPanel()
            }
        }
    }
    
    ScrollDecorator {
        flickableItem: flickable
    }
}
