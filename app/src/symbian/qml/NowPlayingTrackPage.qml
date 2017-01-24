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

MyPage {
    id: root

    title: qsTr("Track details")
    tools: ToolBarLayout {
        BackToolButton {}

        MyToolButton {
            iconSource: "toolbar-view-menu"
            toolTip: qsTr("Options")
            onClicked: popups.open(menu, root)
        }
    }

    MyFlickable {
        id: flickable

        anchors.fill: parent
        contentHeight: flow.height + platformStyle.paddingLarge * 2

        Flow {
            id: flow

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: platformStyle.paddingLarge
            }
            spacing: platformStyle.paddingLarge

            Label {
                id: titleLabel

                width: parent.width
                wrapMode: Text.Wrap
                font.pixelSize: platformStyle.fontSizeLarge
                text: player.metaData.title
            }

            HeaderLabel {
                id: infoHeader

                width: parent.width
                text: qsTr("Details")
            }

            Thumbnail {
                id: thumbnail

                width: column.height
                height: column.height
                source: player.metaData.thumbnailUrl
                placeholderSource: "images/track.svg"
                enabled: false
            }

            Column {
                id: column

                width: parent.width - thumbnail.width - parent.spacing
                spacing: platformStyle.paddingLarge

                Label {
                    id: artistLabel

                    width: parent.width
                    elide: Text.ElideRight
                    text: qsTr("Artist") + ": " + (player.metaData.artist ? player.metaData.artist : qsTr("Unknown"))
                }

                Label {
                    id: genreLabel

                    width: parent.width
                    elide: Text.ElideRight
                    text: qsTr("Genre") + ": " + (player.metaData.genre ? player.metaData.genre : qsTr("Unknown"))
                }

                Label {
                    id: durationLabel

                    width: parent.width
                    elide: Text.ElideRight
                    text: qsTr("Duration") + ": " + (player.metaData.durationString ? player.metaData.durationString : qsTr("Unknown"))
                }
            }

            HeaderLabel {
                id: descriptionHeader

                width: parent.width
                text: qsTr("Description")
            }

            Label {
                id: descriptionLabel

                width: parent.width
                wrapMode: Text.Wrap
                text: player.metaData.description ? player.metaData.description : qsTr("No description")
            }
        }
    }

    ScrollDecorator {
        flickableItem: flickable
    }

    PopupLoader {
        id: popups
    }

    Component {
        id: menu

        MyMenu {
            focusItem: flickable

            MenuLayout {
                MenuItem {
                    text: qsTr("Download")
                    enabled: player.metaData.downloadable
                    onClicked: {
                        switch (player.metaData.service) {
                        case mkresources.SOUNDCLOUD:
                            appWindow.pageStack.push(Qt.resolvedUrl("soundcloud/SoundCloudDownloadPage.qml"))
                            .get(player.queue.itemData(player.currentIndex));
                            break;
                        default:
                            appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginDownloadPage.qml"))
                            .list(player.queue.itemData(player.currentIndex));
                            break;
                        }
                    }
                }

                MenuItem {
                    text: qsTr("Copy URL")
                    onClicked: {
                        clipboard.text = player.metaData.url;
                        infoBanner.information(qsTr("URL copied to clipboard"));
                    }
                }
            }
        }
    }
}
