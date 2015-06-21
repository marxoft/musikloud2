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
        var path;
        var filters = {};
        filters["q"] = query;
        filters["limit"] = MAX_RESULTS;

        if (type == Resources.PLAYLIST) {
            url = Qt.resolvedUrl("SoundCloudPlaylistsPage.qml");
            path = "/playlists";
        }
        else if (type == Resources.ARTIST) {
            url = Qt.resolvedUrl("SoundCloudArtistsPage.qml");
            path = "/users";
        }
        else {
            url = Qt.resolvedUrl("SoundCloudTracksPage.qml");
            path = "/tracks";
        }

        view.currentRow = 1;
        
        if (pageStack.depth > 0) {
            pageStack.clear();
        }
            
        pageStack.push({item: url, properties: {title: qsTr("Search") + " ('" + query + "')"}, immediate: true})
                       .model.get(path, filters);
        return true;
    }
    
    function showResource(resource) {
        if (pageStack.depth > 0) {
            pageStack.clear();
        }

        if (resource.type == Resources.PLAYLIST) {
            pageStack.push({item: Qt.resolvedUrl("SoundCloudPlaylistPage.qml"), immediate: true}).loadPlaylist(resource.id);
        }
        else if (resource.type == Resources.ARTIST) {
            pageStack.push({item: Qt.resolvedUrl("SoundCloudArtistPage.qml"), immediate: true}).loadArtist(resource.id);
        }
        else {
            pageStack.push({item: Qt.resolvedUrl("SoundCloudTrackPage.qml"), immediate: true}).loadTrack(resource.id);
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
                shortcut: "Backspace"
                enabled: pageStack.depth > 1
                onTriggered: pageStack.pop({immediate: true})
            }
        }
    
        Label {
            Layout.fillWidth: true
            elide: Text.ElideRight
            text: (pageStack.currentItem) && (pageStack.currentItem.title) ? pageStack.currentItem.title : "SoundCloud"
        }

        Label {
            text: qsTr("Search for")
        }
        
        ComboBox {
            id: searchTypeSelector
            
            Layout.minimumWidth: 200
            model: SoundCloudSearchTypeModel {
                id: searchTypeModel
            }
            textRole: "name"
            currentIndex: searchTypeModel.match("name", Settings.defaultSearchType(Resources.SOUNDCLOUD))
            onActivated: Settings.setDefaultSearchType(Resources.SOUNDCLOUD, searchTypeModel.data(index, "name"))
        }
        
        TextField {
            id: searchField
            
            Layout.minimumWidth: 300
            placeholderText: qsTr("Search")
            validator: RegExpValidator {
                regExp: /^.+/
            }
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
        model: SoundCloudNavModel {
            id: navModel
        }
        onCurrentRowChanged: {
            if (pageStack.depth > 0) {
                pageStack.clear();
            }
            
            switch (currentRow) {
            case 0:
                pageStack.push({item: Qt.resolvedUrl("SoundCloudAccountsPage.qml"), immediate: true});
                break;
            case 1:
                pageStack.push({item: Qt.resolvedUrl("SoundCloudSearchLabel.qml"), immediate: true});
                break;
            case 2:
                pageStack.push({item: Qt.resolvedUrl("SoundCloudActivitiesPage.qml"), properties: {title: qsTr("Stream")},
                                immediate: true}).model.get("/me/activities", {limit: MAX_RESULTS});
                break;
            case 3:
                pageStack.push({item: Qt.resolvedUrl("SoundCloudTracksPage.qml"), properties: {title: qsTr("Tracks")},
                                immediate: true}).model.get("/me/tracks", {limit: MAX_RESULTS});
                break;
            case 4:
                pageStack.push({item: Qt.resolvedUrl("SoundCloudTracksPage.qml"), properties: {title: qsTr("Favourites")},
                                immediate: true}).model.get("/me/favorites", {limit: MAX_RESULTS});
                break;
            case 5:
                pageStack.push({item: Qt.resolvedUrl("SoundCloudPlaylistsPage.qml"), properties: {title: qsTr("Sets")},
                                immediate: true}).model.get("/me/playlists", {limit: MAX_RESULTS});
                break;
            case 6:
                pageStack.push({item: Qt.resolvedUrl("SoundCloudArtistsPage.qml"), properties: {title: qsTr("Followings")},
                                immediate: true}).model.get("/me/followings", {limit: MAX_RESULTS});
                break;
            default:
                break;
            }
        }
        
        TableViewColumn {
            role: "display"
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
        view.selection.select(1);
        view.currentRow = 1;
    }
}
