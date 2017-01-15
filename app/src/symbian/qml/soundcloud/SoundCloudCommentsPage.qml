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
import QSoundCloud 1.0 as QSoundCloud
import ".."

MyPage {
    id: root
    
    function get(path, filters) {
        commentModel.get(path, filters);
    }
    
    title: qsTr("Comments")
    tools: ToolBarLayout {
        BackToolButton {
            onClicked: commentModel.cancel()
        }
        
        NowPlayingButton {}
        
        MyToolButton {
            id: menuButton
            
            iconSource: "toolbar-view-menu"
            toolTip: qsTr("Options")
            onClicked: popups.open(menu, root)
        }
    }
    
    MyListView {
        id: view

        anchors.fill: parent
        model: SoundCloudCommentModel {
            id: commentModel
            
            onStatusChanged: {
                switch (status) {
                case QSoundCloud.ResourcesRequest.Loading: {
                    root.showProgressIndicator = true;
                    menuButton.enabled = false;
                    label.visible = false;
                    return;
                }
                case QSoundCloud.ResourcesRequest.Failed:
                    infoBanner.information(errorString);
                    break;
                default:
                    break;
                }
                
                root.showProgressIndicator = false;
                menuButton.enabled = true;
                label.visible = (count === 0);
            }
        }
        delegate: CommentDelegate {
            onActivated: appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudArtistPage.qml")).load(artistId)
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
        id: menu
        
        MyMenu {
            focusItem: view
            
            MenuLayout {
                MenuItem {
                    text: qsTr("Add comment")
                    enabled: soundcloud.userId !== ""
                    onClicked: appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudCommentPage.qml"),
                    {trackId: commentModel.data(view.currentIndex, "trackId")})
                }
                
                MenuItem {
                    text: qsTr("Reload")
                    onClicked: commentModel.reload()
                }
            }
        }
    }
}
