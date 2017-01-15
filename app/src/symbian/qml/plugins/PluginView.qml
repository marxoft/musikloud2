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

Item {
    id: root
    
    MyListView {
        id: view
        
        anchors.fill: parent
        model: PluginNavModel {
            id: navModel

            service: settings.currentService
        }
        delegate: TextDelegate {
            subItemIndicator: true
            text: name
            onClicked: {
                if (!value) {
                    appWindow.pageStack.push(Qt.resolvedUrl("PluginSearchPage.qml"));
                    return;
                }
                
                switch (value.type) {
                case mkresources.ARTIST:
                    appWindow.pageStack.push(Qt.resolvedUrl("PluginArtistsPage.qml"), {title: value.label})
                    .list(value.id);
                    break;
                case mkresources.CATEGORY:
                    appWindow.pageStack.push(Qt.resolvedUrl("PluginCategoriesPage.qml"), {title: value.label})
                    .list(value.id);
                    break;
                case mkresources.PLAYLIST:
                    appWindow.pageStack.push(Qt.resolvedUrl("PluginPlaylistsPage.qml"), {title: value.label})
                    .list(value.id);
                    break;
                case mkresources.TRACK:
                    appWindow.pageStack.push(Qt.resolvedUrl("PluginTracksPage.qml"), {title: value.label})
                    .list(value.id);
                    break;
                default:
                    break;
                }
            }
        }
    }
}
