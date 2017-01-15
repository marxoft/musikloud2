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
        
        if ((resource) && (resource.service === settings.currentService)) {
            switch (resource.type) {
            case mkresources.ARTIST:
                appWindow.pageStack.replace(Qt.resolvedUrl("PluginArtistPage.qml")).load(resource.id);
                return;
            case mkresources.PLAYLIST:
                appWindow.pageStack.replace(Qt.resolvedUrl("PluginPlaylistPage.qml")).load(resource.id);
                return;
            case mkresources.TRACK:
                appWindow.pageStack.replace(Qt.resolvedUrl("PluginTrackPage.qml")).load(resource.id);
                return;
            default:
                break;
            }
        }
        
        settings.addSearch(searchField.text);
        settings.setDefaultSearchType(settings.currentService,
        searchTypeModel.data(searchTypeSelector.selectedIndex, "name"));
        
        switch (searchTypeSelector.value.type) {
        case mkresources.ARTIST:
            appWindow.pageStack.replace(Qt.resolvedUrl("PluginArtistsPage.qml"),
            {title: qsTr("Search") + " '" + searchField.text + "'"})
            .search(searchField.text, searchTypeSelector.value.order);
            break;
        case mkresources.CATEGORY:
            appWindow.pageStack.replace(Qt.resolvedUrl("PluginCategoriesPage.qml"),
            {title: qsTr("Search") + " '" + searchField.text + "'"})
            .search(searchField.text, searchTypeSelector.value.order);
            break;
        case mkresources.PLAYLIST:
            appWindow.pageStack.replace(Qt.resolvedUrl("PluginPlaylistsPage.qml"),
            {title: qsTr("Search") + " '" + searchField.text + "'"})
            .search(searchField.text, searchTypeSelector.value.order);
            break;
        case mkresources.TRACK:
            appWindow.pageStack.replace(Qt.resolvedUrl("PluginTracksPage.qml"),
            {title: qsTr("Search") + " '" + searchField.text + "'"})
            .search(searchField.text, searchTypeSelector.value.order);
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
                model: PluginSearchTypeModel {
                    id: searchTypeModel
                    
                    service: settings.currentService
                }
                selectedIndex: Math.max(0, searchTypeModel.match("name",
                settings.defaultSearchType(settings.currentService)))
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
