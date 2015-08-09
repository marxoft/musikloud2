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

    property alias model: artistModel

    title: qsTr("Artists")
    tools: ToolBarLayout {

        BackToolIcon {}
        
        NowPlayingButton {}

        MyToolIcon {
            platformIconId: "toolbar-refresh"
            enabled: artistModel.status != ResourcesRequest.Loading
            onClicked: artistModel.reload()
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        cacheBuffer: 400
        interactive: count > 0
        highlightFollowsCurrentItem: false
        model: PluginArtistModel {
            id: artistModel

            service: Settings.currentService
            onStatusChanged: if (status == ResourcesRequest.Failed) infoBanner.showMessage(errorString);
        }
        header: PageHeader {
            title: root.title
            showProgressIndicator: artistModel.status == ResourcesRequest.Loading
        }
        delegate: ArtistDelegate {
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PluginArtistPage.qml")).load(artistModel.get(index))
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
            text: qsTr("No artists found")
            visible: (artistModel.status >= ResourcesRequest.Ready) && (artistModel.count == 0)
        }
    }

    ScrollDecorator {
        flickableItem: view
    }    
}
