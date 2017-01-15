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

MyPage {
    id: root

    title: qsTr("Playback settings")
    tools: ToolBarLayout {
        BackToolButton {}
    }

    KeyNavFlickable {
        id: flickable

        anchors.fill: parent
        contentHeight: inputContext.visible ? height : flow.height + platformStyle.paddingLarge * 2

        Flow {
            id: flow

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: platformStyle.paddingLarge
            }
            spacing: platformStyle.paddingLarge

            MySwitch {
                id: stopSwitch
                
                width: parent.width
                text: qsTr("Stop after current track")
                checked: player.stopAfterCurrentTrack
                visible: !inputContext.visible
                onCheckedChanged: player.stopAfterCurrentTrack = checked
            }
            
            MySwitch {
                id: sleepSwitch
                
                width: parent.width
                text: qsTr("Enable sleep timer")
                checked: player.sleepTimerEnabled
                visible: !inputContext.visible
                onCheckedChanged: player.sleepTimerEnabled = checked
            }
            
            Label {
                width: parent.width - sleepDurationField.width - parent.spacing
                height: sleepDurationField.height
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
                text: qsTr("Sleep timer duration (mins)")
                visible: sleepDurationField.visible
            }
            
            MyTextField {
                id: sleepDurationField
                
                inputMethodHints: Qt.ImhDigitsOnly
                text: settings.sleepTimerDuration
                visible: (!inputContext.visible) || (focus)
                onTextChanged: settings.sleepTimerDuration = parseInt(text)
            }
            
            Label {
                width: parent.width
                elide: Text.ElideRight
                color: platformStyle.colorNormalMid
                text: player.sleepTimerRemainingString + " " + qsTr("remaining")
                visible: (!inputContext.visible) && (player.sleepTimerEnabled)
            }
        }
    }
}
