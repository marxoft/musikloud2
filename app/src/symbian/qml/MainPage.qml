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
import "plugins"
import "soundcloud"

MyPage {
    id: root
    
    function setService(service) {
        if (!service) {
            service = mkresources.SOUNDCLOUD;
        }
        
        settings.currentService = service;
        title = serviceModel.data(serviceModel.match("value", service), "name");
        
        switch (service) {
        case mkresources.SOUNDCLOUD:
            loader.sourceComponent = soundcloudView;
            break;
        default:
            loader.sourceComponent = pluginView;
            break;
        }
    }
    
    title: "MusiKloud2"
    tools: ToolBarLayout {
        BackToolButton {}
        
        NowPlayingButton {}
        
        MyToolButton {
            iconSource: "toolbar-view-menu"
            toolTip: qsTr("Options")
            onClicked: popups.open(menu, root)
        }
    }
    
    ServiceModel {
        id: serviceModel
    }
    
    Loader {
        id: loader
        
        anchors.fill: parent
    }
    
    PopupLoader {
        id: popups
    }
    
    Component {
        id: soundcloudView
        
        SoundCloudView {
            anchors.fill: parent
        }
    }
    
    Component {
        id: pluginView
        
        PluginView {
            anchors.fill: parent
        }
    }
    
    Component {
        id: serviceDialog
        
        ValueDialog {
            focusItem: loader.item
            titleText: qsTr("Service")
            model: serviceModel
            value: settings.currentService
            onAccepted: root.setService(value)
        }
    }
    
    Component {
        id: menu
        
        MyMenu {
            focusItem: loader.item
            
            MenuLayout {
                MenuItem {
                    text: qsTr("Service")
                    platformSubItemIndicator: true
                    onClicked: popups.open(serviceDialog, root)
                }
                
                MenuItem {
                    text: qsTr("Play folder")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PlayFolderPage.qml"))
                }
                
                MenuItem {
                    text: qsTr("Queue folder")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("AddFolderPage.qml"))
                }
                
                MenuItem {
                    text: qsTr("Play URL")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PlayUrlPage.qml"))
                }
                
                MenuItem {
                    text: qsTr("Queue URL")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("AddUrlPage.qml"))
                }
                
                MenuItem {
                    text: qsTr("Transfers")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("TransfersPage.qml"))
                }
                
                MenuItem {
                    text: qsTr("Settings")
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
                }
            }
        }
    }
}
