/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import MusiKloud 2.0
import QSoundCloud 1.0 as QSoundCloud
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    title: qsTr("Accounts")
    tools: ToolBarLayout {

        BackToolIcon {}

        MyToolIcon {
            platformIconId: "toolbar-add"
            onClicked: {
                loader.sourceComponent = authDialog;
                loader.item.open();
            }
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        interactive: count > 0
        highlightFollowsCurrentItem: false
        enabled: !root.showProgressIndicator
        model: SoundCloudAccountModel {
            id: accountModel
        }
        header: PageHeader {
            title: root.title
            showProgressIndicator: (authRequest.status == QSoundCloud.AuthenticationRequest.Loading)
                                   || (userRequest.status == QSoundCloud.AuthenticationRequest.Loading)
        }
        delegate: LabelDelegate {
            text: username
            isSelected: active
            onClicked: infoBanner.showMessage(accountModel.selectAccount(index)
                                              ? qsTr("You have selected account") + " '" + username + "'"
                                              : accountModel.errorString)
            onPressAndHold: {
                view.currentIndex = index;
                contextMenu.open();
            }
        }
        
        Label {
            anchors {
                fill: parent
                margins: UI.PADDING_DOUBLE
            }
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
            font.pixelSize: 60
            color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND
            text: qsTr("No accounts")
            visible: accountModel.count == 0
        }
    }

    ScrollDecorator {
        flickableItem: view
    }    

    ContextMenu {
        id: contextMenu

        MenuLayout {

            MenuItem {
                text: qsTr("Remove")
                onClicked: infoBanner.showMessage(accountModel.removeAccount(view.currentIndex)
                           ? qsTr("Account removed. Please visit the SoundCloud website to revoke the access token")
                           : accountModel.errorString)
            }
        }
    }

    Loader {
        id: loader
    }

    QSoundCloud.AuthenticationRequest {
        id: authRequest

        clientId: SoundCloud.clientId
        clientSecret: SoundCloud.clientSecret
        redirectUri: SoundCloud.redirectUri
        scopes: SoundCloud.scopes
        onFinished: {
            if (status == QSoundCloud.AuthenticationRequest.Ready) {
                if (result.access_token) {
                    userRequest.accessToken = result.access_token;
                    userRequest.refreshToken = (result.refresh_token ? result.refresh_token : "");
                    userRequest.get("/me");
                    return;
                }
            }

            infoBanner.showMessage(SoundCloud.getErrorString(result));
        }
    }
    
    QSoundCloud.ResourcesRequest {
        id: userRequest

        clientId: SoundCloud.clientId
        clientSecret: SoundCloud.clientSecret
        onFinished: {
            if (status == QSoundCloud.ResourcesRequest.Ready) {
                if (accountModel.addAccount(result.id, result.username, accessToken, refreshToken,
                                            SoundCloud.scopes.join(" "))) {
                    infoBanner.showMessage(qsTr("You are signed in to account") + " '" + result.username + "'");
                }
                else {
                    infoBanner.showMessage(accountModel.errorString);
                }

                return;
            }

            messageBox.showError(SoundCloud.getErrorString(result));
        }
    }

    Component {
        id: authDialog

        SoundCloudAuthDialog {
            onCodeReady: authRequest.exchangeCodeForAccessToken(code)
        }
    }
}
