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
import MusiKloud 2.0
import ".."

Page {
    id: root
    
    function search(query, type, order) {
        var url;

        if (type == Resources.PLAYLIST) {
            url = Qt.resolvedUrl("PluginPlaylistsPage.qml");
        }
        else if (type == Resources.ARTIST) {
            url = Qt.resolvedUrl("PluginArtistsPage.qml");
        }
        else {
            url = Qt.resolvedUrl("PluginTracksPage.qml");
        }

        view.selection.clear();
        view.selection.select(0);
        view.currentRow = 0;
        
        if (pageStack.depth > 0) {
            pageStack.clear();
        }
        
        pageStack.push({item: url, properties: {title: qsTr("Search") + " ('" + query + "')"}, immediate: true})
                       .model.search(query, order);
        return true;
    }
    
    function showResource(resource) {
        if (pageStack.depth > 0) {
            pageStack.clear();
        }

        if (resource.type == Resources.PLAYLIST) {
            pageStack.push({item: Qt.resolvedUrl("PluginPlaylistPage.qml"), immediate: true}).loadPlaylist(resource.id);
        }
        else if (resource.type == Resources.ARTIST) {
            pageStack.push({item: Qt.resolvedUrl("PluginArtistPage.qml"), immediate: true}).loadArtist(resource.id);
        }
        else {
            pageStack.push({item: Qt.resolvedUrl("PluginTrackPage.qml"), immediate: true}).loadTrack(resource.id);
        }
        
        return true;
    }
    
    tools: ToolBarLayout {
    
        ToolButton {
            id: backButton
            
            action: Action {
                id: backAction
                
                text: qsTr("Go back")
                iconName: "go-previous"
                enabled: pageStack.depth > 1
                onTriggered: pageStack.pop({immediate: true})
            }
        }
    
        Label {
            Layout.fillWidth: true
            elide: Text.ElideRight
            text: (pageStack.currentItem) && (pageStack.currentItem.title) ? pageStack.currentItem.title
                                                                           : Settings.currentService
        }

        Label {
            text: qsTr("Search for")
            enabled: searchTypeModel.count > 0
        }
        
        ComboBox {
            id: searchTypeSelector
            
            Layout.minimumWidth: 200
            model: PluginSearchTypeModel {
                id: searchTypeModel
                
                service: Settings.currentService
            }
            textRole: "name"
            currentIndex: searchTypeModel.match("name", Settings.defaultSearchType(Resources.SOUNDCLOUD))
            enabled: searchTypeModel.count > 0
            onActivated: Settings.setDefaultSearchType(Resources.SOUNDCLOUD, searchTypeModel.data(index, "name"))
        }
        
        TextField {
            id: searchField
            
            Layout.minimumWidth: 300
            placeholderText: qsTr("Search")
            validator: RegExpValidator {
                regExp: /^.+/
            }
            enabled: searchTypeModel.count > 0
            onAccepted: {
                root.search(text, searchTypeModel.data(searchTypeSelector.currentIndex, "value").type,
                            searchTypeModel.data(searchTypeSelector.currentIndex, "value").order);
                text = "";
            }
        }    
    }
    
    TableView {
        id: view
        
        width: 150
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        alternatingRowColors: false
        headerVisible: false
        model: PluginNavModel {
            id: navModel
            
            service: Settings.currentService
        }
        onCurrentRowChanged: {
            if (pageStack.depth > 0) {
                pageStack.clear();
            }
            
            switch (currentRow) {
            case 0:
                if (searchTypeModel.count > 0) {
                    return;
                }
                
                break;
            default:
                break;
            }
            
            var value = navModel.data(currentRow, "value");
            
            if (value.type == Resources.CATEGORY) {
                pageStack.push({item: Qt.resolvedUrl("PluginCategoriesPage.qml"), properties: {title: value.name},
                               immediate: true}).model.list(value.id);
            }
            else if (value.type == Resources.PLAYLIST) {
                pageStack.push({item: Qt.resolvedUrl("PluginPlaylistsPage.qml"), properties: {title: value.name},
                               immediate: true}).model.list(value.id);
            }
            else if (value.type == Resources.ARTIST) {
                pageStack.push({item: Qt.resolvedUrl("PluginArtistsPage.qml"), properties: {title: value.name},
                               immediate: true}).model.list(value.id);
            }
            else {
                pageStack.push({item: Qt.resolvedUrl("PluginTracksPage.qml"), properties: {title: value.name},
                               immediate: true}).model.list(value.id);
            }
        }
        
        TableViewColumn {
            role: "name"
            title: qsTr("View")
        }
    }
    
    StackView {
        id: pageStack
        
        anchors {
            left: view.right
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
    }
    
    Component.onCompleted: {
        if (navModel.count > 0) {
            view.selection.select(0);
            view.currentRow = 0;
        }
    }
}
