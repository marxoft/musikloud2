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

Tab {
    id: root

    property alias model: commentModel

    title: qsTr("Comments")
    tools: ToolBarLayout {

        BackToolIcon {}
        
        NowPlayingButton {}

        MyToolIcon {
            platformIconId: "toolbar-view-menu"
            onClicked: menu.open()
        }
    }
    
    Menu {
        id: menu
        
        MenuItem {
            text: qsTr("Add comment")
            enabled: SoundCloud.userId != ""
            onClicked: {
                dialogLoader.sourceComponent = commentDialog;
                dialogLoader.item.trackId = track.id;
                dialogLoader.item.open();
            }
        }
        
        MenuItem {
            text: qsTr("Reload")
            enabled: commentModel.status != QSoundCloud.ResourcesRequest.Loading
            onClicked: commentModel.reload()
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        cacheBuffer: 400
        interactive: count > 0
        highlightFollowsCurrentItem: false
        model: SoundCloudCommentModel {
            id: commentModel

            onStatusChanged: if (status == QSoundCloud.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
        }
        header: PageHeader {
            title: root.title
            showProgressIndicator: commentModel.status == QSoundCloud.ResourcesRequest.Loading
        }
        delegate: CommentDelegate {
            onThumbnailClicked: appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudArtistPage.qml")).load(artistId)
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
            text: qsTr("No comments found")
            visible: (commentModel.status >= QSoundCloud.ResourcesRequest.Ready) && (commentModel.count == 0)
        }
    }

    ScrollDecorator {
        flickableItem: view
    }

    Loader {
        id: dialogLoader
    }

    Component {
        id: commentDialog

        SoundCloudCommentDialog {}
    }
}
