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

EditPage {
    id: root
    
    function search() {
        if (!searchField.text) {
            return;
        }
        
        var resource = mkresources.getResourceFromUrl(searchField.text);
        
        if ((resource) && (resource.service === mkresources.SOUNDCLOUD)) {
            switch (resource.type) {
            case mkresources.ARTIST:
                appWindow.pageStack.replace(Qt.resolvedUrl("SoundCloudArtistPage.qml")).load(resource.id);
                return;
            case mkresources.PLAYLIST:
                appWindow.pageStack.replace(Qt.resolvedUrl("SoundCloudPlaylistPage.qml")).load(resource.id);
                return;
            case mkresources.TRACK:
                appWindow.pageStack.replace(Qt.resolvedUrl("SoundCloudTrackPage.qml")).load(resource.id);
                return;
            default:
                break;
            }
        }
        
        settings.addSearch(searchField.text);
        settings.setDefaultSearchType(mkresources.SOUNDCLOUD,
        searchTypeModel.data(searchTypeSelector.selectedIndex, "name"));
        
        switch (searchTypeSelector.value.type) {
        case mkresources.ARTIST:
            appWindow.pageStack.replace(Qt.resolvedUrl("SoundCloudArtistsPage.qml"),
            {title: qsTr("Search") + " '" + searchField.text + "'"})
            .get("/users", {q: searchField.text, limit: MAX_RESULTS});
            break;
        case mkresources.PLAYLIST:
            appWindow.pageStack.replace(Qt.resolvedUrl("SoundCloudPlaylistsPage.qml"),
            {title: qsTr("Search") + " '" + searchField.text + "'"})
            .get("/playlists", {q: searchField.text, limit: MAX_RESULTS});
            break;
        case mkresources.TRACK:
            appWindow.pageStack.replace(Qt.resolvedUrl("SoundCloudTracksPage.qml"),
            {title: qsTr("Search") + " '" + searchField.text + "'"})
            .get("/tracks", {q: searchField.text, limit: MAX_RESULTS});
            break;
        default:
            break;
        }
    }
    
    title: qsTr("Search")
    acceptable: searchField.text !== ""
    
    KeyNavFlickable {
        id: flickable
        
        anchors.fill: parent
        contentHeight: column.height + platformStyle.paddingLarge
        
        Column {
            id: column
            
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: platformStyle.paddingLarge
            }
            spacing: platformStyle.paddingLarge
            
            Label {
                width: parent.width
                elide: Text.ElideRight
                text: qsTr("Query")
            }
            
            MyTextField {
                id: searchField
                
                width: parent.width
                onAccepted: root.accepted()
            }
            
            ValueSelector {
                id: searchTypeSelector
                
                x: -platformStyle.paddingLarge
                width: parent.width + platformStyle.paddingLarge * 2
                title: qsTr("Search for")
                model: SoundCloudSearchTypeModel {
                    id: searchTypeModel
                }
                selectedIndex: Math.max(0, searchTypeModel.match("name",
                settings.defaultSearchType(mkresources.SOUNDCLOUD)))
            }
        }
    }
    
    ScrollDecorator {
        flickableItem: flickable
    }
    
    PopupLoader {
        id: popups
    }
    
    states: State {
        name: "inputContextVisible"
        when: inputContext.visible
        
        PropertyChanges {
            target: flickable
            contentHeight: flickable.height
        }
        
        PropertyChanges {
            target: searchTypeSelector
            visible: false
        }
    }
    
    onAccepted: search()
}
