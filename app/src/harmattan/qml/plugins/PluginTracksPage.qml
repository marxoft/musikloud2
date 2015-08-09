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
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    property alias model: trackModel

    title: qsTr("Tracks")
    tools: ToolBarLayout {

        BackToolIcon {}
        
        NowPlayingButton {}

        MyToolIcon {
            platformIconId: "toolbar-refresh"
            enabled: trackModel.status != ResourcesRequest.Loading
            onClicked: trackModel.reload()
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        cacheBuffer: 400
        interactive: count > 0
        highlightFollowsCurrentItem: false
        model: PluginTrackModel {
            id: trackModel

            service: Settings.currentService
            onStatusChanged: if (status == ResourcesRequest.Failed) infoBanner.showMessage(errorString);
        }
        header: PageHeader {
            title: root.title
            showProgressIndicator: trackModel.status == ResourcesRequest.Loading
        }
        delegate: TrackDelegate {
            onThumbnailClicked: {
                player.clearQueue();
                player.addTrack(trackModel.get(index));
                appWindow.pageStack.push(Qt.resolvedUrl("../NowPlayingPage.qml"), {autoPlay: true});
            }
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PluginTrackPage.qml")).load(trackModel.get(index))
            onPressAndHold: {
                view.currentIndex = -1;
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
            text: qsTr("No tracks found")
            visible: (trackModel.status >= ResourcesRequest.Ready) && (trackModel.count == 0)
        }
    }

    ScrollDecorator {
        flickableItem: view
    }    

    ContextMenu {
        id: contextMenu

        MenuLayout {
            
            MenuItem {
                text: qsTr("Add to queue")
                onClicked: {
                    player.addTrack(trackModel.get(view.currentIndex));
                    infoBanner.showMessage(qsTr("1 track added to queue"));
                }
            }

            MenuItem {
                text: qsTr("Download")
                enabled: (view.currentIndex >= 0) && (trackModel.data(view.currentIndex, "downloadable"))
                onClicked: {
                    dialogLoader.sourceComponent = downloadDialog;
                    dialogLoader.item.resourceId = trackModel.data(view.currentIndex, "id");
                    dialogLoader.item.resourceTitle = trackModel.data(view.currentIndex, "title");
                    dialogLoader.item.streamUrl = trackModel.data(view.currentIndex, "streamUrl");
                    dialogLoader.item.open();
                }
            }

            MenuItem {
                text: qsTr("Share")
                onClicked: if (!ShareUi.shareTrack(trackModel.get(view.currentIndex)))
                               infoBanner.showMessage(qsTr("Unable to share track"));
            }
        }
    }

    Loader {
        id: dialogLoader
    }

    Component {
        id: downloadDialog

        PluginDownloadDialog {}
    }
}
