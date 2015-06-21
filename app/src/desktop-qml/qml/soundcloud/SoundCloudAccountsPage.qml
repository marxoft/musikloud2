/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
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

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtWebKit.experimental 1.0
import MusiKloud 2.0
import QSoundCloud 1.0 as QSoundCloud
import ".."

Page {
    id: root
    
    title: qsTr("Accounts")
        
    Loader {
        id: viewLoader
        
        width: 150
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        sourceComponent: accountModel.count > 0 ? accountsView : undefined
    }
    
    Component {
        id: accountsView
        
        Item {
            width: 150
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            
            TableView {
                id: view
            
                anchors.fill: parent
                alternatingRowColors: false
                headerVisible: false
                model: accountModel
                itemDelegate: LabelDelegate {
                    onPressed: {
                        view.forceActiveFocus();
                        view.selection.clear();
                        view.selection.select(styleData.row);
                        view.currentRow = styleData.row;
                    }
                    onDoubleClicked: {
                        if (accountModel.selectAccount(styleData.row)) {
                            statusBar.showMessage(qsTr("You have selected account") + " '"
                                                  + accountModel.data(styleData.row, "username") + "'");
                        }
                        else {
                            messageBox.showError(accountModel.errorString);
                        }
                    }
                    onRightClicked: contextMenu.popup()
                }
                
                TableViewColumn {
                    role: "username"
                    title: qsTr("Accounts")
                }
            }
        
            Menu {
                id: contextMenu
            
                MenuItem {
                    text: qsTr("Remove")
                    iconName: "edit-delete"
                    onTriggered: {
                        var username = accountModel.data(view.currentRow, "username");
                    
                        if (accountModel.removeAccount(view.currentRow)) {
                            messageBox.showMessage(qsTr("Account") + " '" + username + "' " + qsTr("removed"));
                        }
                        else {
                            messageBox.showError(accountModel.errorString);
                        }
                    }
                }
            }
        }
    }
    
    ColumnLayout {
        id: column
        
        anchors {
            left: parent.left
            leftMargin: viewLoader.item ? viewLoader.item.width : 0
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        
        Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Sign in to your SoundCloud account")
        }
        
        Button {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Sign in")
            enabled: authLoader.sourceComponent == undefined
            onClicked: authLoader.sourceComponent = authView
        }
        
        Loader {
            id: authLoader
            
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
    
    Component {
        id: authView
        
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            WebView {
                id: webView

                anchors.fill: parent
                onUrlChanged: {
                    var s = url.toString();

                    if (/code=/i.test(s)) {
                        authRequest.exchangeCodeForAccessToken(s.split("code=")[1].split("#")[0]);
                        authLoader.sourceComponent = undefined;
                    }
                }
            
                Component.onCompleted: url = SoundCloud.authUrl()
            }
        }
    }
    
    SoundCloudAccountModel {
        id: accountModel
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

            messageBox.showError(SoundCloud.getErrorString(result));
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
                    statusBar.showMessage(qsTr("You are signed in to account") + " '" + result.username + "'");
                }
                else {
                    messageBox.showError(accountModel.errorString);
                }

                return;
            }

            messageBox.showError(SoundCloud.getErrorString(result));
        }
    }
}
