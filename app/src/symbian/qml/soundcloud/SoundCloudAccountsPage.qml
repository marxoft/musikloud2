/*
 * Copyright (C) 2017 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1
import com.nokia.symbian 1.1
import MusiKloud 2.0
import QSoundCloud 1.0 as QSoundCloud
import ".."

MyPage {
    id: root

    title: qsTr("Accounts")
    tools: ToolBarLayout {
        BackToolButton {
            onClicked: {
                authRequest.cancel();
                userRequest.cancel();
            }
        }
        
        MyToolButton {
            iconSource: "toolbar-add"
            onClicked: {
                var page = appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudAuthPage.qml"));
                page.accepted.connect(function() { authRequest.exchangeCodeForAccessToken(page.code); });
            }
        }
    }

    MyListView {
        id: view

        anchors.fill: parent
        model: SoundCloudAccountModel {
            id: accountModel
        }
        delegate: AccountDelegate {
            onClicked: infoBanner.information(accountModel.selectAccount(index)
            ? qsTr("You have selected account") + " '" + username + "'" : accountModel.errorString)
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
        text: qsTr("No accounts")
        visible: accountModel.count === 0
    }
    
    PopupLoader {
        id: popups
    }
    
    Component {
        id: contextMenu
        
        MyContextMenu {
            focusItem: view

            MenuLayout {
                MenuItem {
                    text: qsTr("Remove")
                    onClicked: infoBanner.information(accountModel.removeAccount(view.currentIndex)
                    ? qsTr("Account removed. Please visit the SoundCloud website to revoke the access token")
                    : accountModel.errorString)
                }
            }
        }
    }
    
    QSoundCloud.AuthenticationRequest {
        id: authRequest

        clientId: soundcloud.clientId
        clientSecret: soundcloud.clientSecret
        redirectUri: soundcloud.redirectUri
        scopes: soundcloud.scopes
        onStatusChanged: {
            switch (status) {
            case QSoundCloud.AuthenticationRequest.Loading:
                root.showProgressIndicator = true;
                return;
            case QSoundCloud.AuthenticationRequest.Ready:
                if (result.access_token) {
                    userRequest.accessToken = result.access_token;
                    userRequest.refreshToken = (result.refresh_token ? result.refresh_token : "");
                    userRequest.get("/me");
                    return;
                }

                break;
            case QSoundCloud.AuthenticationRequest.Failed:
                infoBanner.information(soundcloud.getErrorString(result));
                break;
            default:
                break;
            }

            root.showProgressIndicator = false;
        }
    }
    
    QSoundCloud.ResourcesRequest {
        id: userRequest

        clientId: soundcloud.clientId
        clientSecret: soundcloud.clientSecret
        onStatusChanged: {
            switch (status) {
            case QSoundCloud.ResourcesRequest.Loading:
                root.showProgressIndicator = true;
                return;
            case QSoundCloud.ResourcesRequest.Ready:
                if (accountModel.addAccount(result.id, result.username, accessToken, refreshToken,
                                            soundcloud.scopes.join(" "))) {
                    infoBanner.information(qsTr("You are signed in to account") + " '" + result.username + "'");
                }
                else {
                    infoBanner.information(accountModel.errorString);
                }

                break;
            case QSoundCloud.ResourcesRequest.Failed:
                infoBanner.information(soundcloud.getErrorString(result));
                break;
            default:
                break;
            }

            root.showProgressIndicator = false;
        }
    }
}
