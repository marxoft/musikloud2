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
import ".."

MyPage {
    id: root
    
    function list(id) {
        artistModel.list(id);
    }
    
    function search(query, order) {
        artistModel.search(query, order);
    }
    
    title: qsTr("Artists")
    tools: ToolBarLayout {
        BackToolButton {
            onClicked: artistModel.cancel()
        }
        
        NowPlayingButton {}
        
        MyToolButton {
            id: reloadButton
            
            iconSource: "toolbar-refresh"
            toolTip: qsTr("Reload")
            onClicked: artistModel.reload()
        }
    }
    
    MyListView {
        id: view

        anchors.fill: parent
        model: PluginArtistModel {
            id: artistModel

            service: settings.currentService
            onStatusChanged: {
                switch (status) {
                case ResourcesRequest.Loading: {
                    root.showProgressIndicator = true;
                    reloadButton.enabled = false;
                    label.visible = false;
                    return;
                }
                case ResourcesRequest.Failed:
                    infoBanner.information(errorString);
                    break;
                default:
                    break;
                }
                
                root.showProgressIndicator = false;
                reloadButton.enabled = true;
                label.visible = (count === 0);
            }
        }
        delegate: ArtistDelegate {
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PluginArtistPage.qml")).load(artistModel.get(index))
            onPressAndHold: if (actions.length) popups.open(contextMenu, root);
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
        text: qsTr("No results")
        visible: false
    }
    
    PopupLoader {
        id: popups
    }
    
    Component {
        id: contextMenu
        
        MyContextMenu {
            focusItem: view
            
            MenuLayout {
                Repeater {
                    model: artistModel.data(view.currentIndex, "actions")
                    
                    MenuItem {
                        text: modelData.label
                        onClicked: {
                            switch (modelData.method) {
                            case "del":
                                artistModel.get(view.currentIndex).del(modelData.type, modelData.id);
                                break;
                            case "insert":
                                artistModel.get(view.currentIndex).insert(modelData.type, modelData.id);
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}
