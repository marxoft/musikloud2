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
import "plugins"
import "soundcloud"

MyPage {
    id: root

    function search(service, query, type, order) {
        setService(service);
        appWindow.pageStack.pop(null, true);
        viewLoader.item.search(query, type, order);
    }

    function setService(service) {
        if (!service) {
            return;
        }

        Settings.currentService = service;
        
        if (service == Resources.SOUNDCLOUD) {
            viewLoader.sourceComponent = soundcloudView;
        }
        else {
            viewLoader.sourceComponent = pluginView;
        }
    }

    function showResource(resource) {
        if (!resource.service) {
            return false;
        }

        setService(resource.service);
        appWindow.pageStack.pop(null, true);
        MainWindow.raise();
        viewLoader.item.showResource(resource);
        return true;
    }

    function showResourceFromUrl(url) {
        return showResource(Resources.getResourceFromUrl(url));
    }
    
    title: Settings.currentService ? serviceModel.data(serviceModel.match("value", Settings.currentService), "name")
                                   : "MusiKloud2"
    tools: ToolBarLayout {
        MyToolIcon {
            platformIconId: "toolbar-settings"
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
        }
        
        NowPlayingButton {}

        MyToolIcon {
            platformIconId: "toolbar-list"
            onClicked: {
                dialogLoader.sourceComponent = serviceDialog;
                dialogLoader.item.open();
            }
        }
    }

    ServiceModel {
        id: serviceModel
    }
    
    Loader {
        id: viewLoader
        
        anchors.fill: parent
    }
    
    Loader {
        id: dialogLoader        
    }
    
    Component {
        id: soundcloudView        
        
        ListView {

            function search(query, type, order) {
                var filters = {};
                filters["q"] = query;
                filters["limit"] = MAX_RESULTS;

                if (type == Resources.ARTIST) {
                    appWindow.pageStack.push(Qt.resolvedUrl("soundcloud/SoundCloudArtistsPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.get("/users", filters);
                }
                else {
                    appWindow.pageStack.push(Qt.resolvedUrl("soundcloud/SoundCloudTracksPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.get("/tracks", filters);
                }
            }

            function showResource(resource) {
                var url;

                if (resource.type == Resources.PLAYLIST) {
                    url = Qt.resolvedUrl("soundcloud/SoundCloudPlaylistPage.qml");
                }
                else if (resource.type == Resources.ARTIST) {
                    url = Qt.resolvedUrl("soundcloud/SoundCloudArtistPage.qml");
                }
                else {
                    url = Qt.resolvedUrl("soundcloud/SoundCloudTrackPage.qml");
                }

                appWindow.pageStack.push(url).load(resource.id);
            }

            anchors.fill: parent
            model: SoundCloudNavModel {}
            header: PageHeader {
                title: root.title
            }
            delegate: DrillDownDelegate {
                onClicked: {
                    switch (index) {
                    case 0:
                        appWindow.pageStack.push(Qt.resolvedUrl("soundcloud/SoundCloudAccountsPage.qml"));
                        break;
                    case 1: {
                        dialogLoader.sourceComponent = soundcloudSearchDialog;
                        dialogLoader.item.open();
                        break;
                    }
                    case 2:
                        appWindow.pageStack.push(Qt.resolvedUrl("soundcloud/SoundCloudTracksPage.qml"), {title: qsTr("Tracks")})
                        .model.get("/me/tracks", {limit: MAX_RESULTS});
                        break;
                    case 3:
                        appWindow.pageStack.push(Qt.resolvedUrl("soundcloud/SoundCloudTracksPage.qml"), {title: qsTr("Favourites")})
                        .model.get("/me/favorites", {limit: MAX_RESULTS});
                        break;
                    case 4:
                        appWindow.pageStack.push(Qt.resolvedUrl("soundcloud/SoundCloudPlaylistsPage.qml"), {title: qsTr("Playlists")})
                        .model.get("/me/playlists", {limit: MAX_RESULTS});
                        break;
                    case 5:
                        appWindow.pageStack.push(Qt.resolvedUrl("soundcloud/SoundCloudArtistsPage.qml"), {title: qsTr("Followings")})
                        .model.get("/me/followings", {limit: MAX_RESULTS});
                        break;
                    }
                }
            }

            Connections {
                target: SoundCloud
                onCommentAdded: infoBanner.showMessage(qsTr("Your comment has been added"))
                onArtistFollowed: infoBanner.showMessage(qsTr("You have followed") + " " + artist.name)
                onArtistUnfollowed: infoBanner.showMessage(qsTr("You have unfollowed") + " " + artist.name)
                onTrackFavourited: infoBanner.showMessage("'" + track.title + "' " + qsTr("added to favourites"))
                onTrackUnfavourited: infoBanner.showMessage("'" + track.title + "' " + qsTr("removed from favourites"))
            }
        }        
    }
    
    Component {
        id: pluginView
        
        ListView {

            function search(query, type, order) {
                if (type == Resources.PLAYLIST) {
                    appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginPlaylistsPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.search(query, order);
                }
                else if (type == Resources.ARTIST) {
                    appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginArtistsPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.search(query, order);
                }
                else {
                    appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginTracksPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.search(query, order);
                }
            }
            
            function showResource(resource) {
                var url;

                if (resource.type == Resources.PLAYLIST) {
                    url = Qt.resolvedUrl("plugins/PluginPlaylistPage.qml");
                }
                else if (resource.type == Resources.ARTIST) {
                    url = Qt.resolvedUrl("plugins/PluginArtistPage.qml");
                }
                else {
                    url = Qt.resolvedUrl("plugins/PluginTrackPage.qml");
                }

                appWindow.pageStack.push(url).load(resource.id);
            }

            anchors.fill: parent
            model: PluginNavModel {
                service: Settings.currentService
            }
            header: PageHeader {
                title: root.title
            }
            delegate: DrillDownDelegate {
                text: name
                onClicked: {
                    if (!value) {
                        dialogLoader.sourceComponent = pluginSearchDialog;
                        dialogLoader.item.open();
                    }
                    else if (value.type == Resources.CATEGORY) {
                        appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginCategoriesPage.qml"),
                                                 {title: value.name}).model.list(value.id);
                    }
                    else if (value.type == Resources.PLAYLIST) {
                        appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginPlaylistsPage.qml"),
                                                 {title: value.name}).model.list(value.id);
                    }
                    else if (value.type == Resources.ARTIST) {
                        appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginArtistsPage.qml"),
                                                 {title: value.name}).model.list(value.id);
                    }
                    else {
                        appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginTracksPage.qml"),
                                                 {title: value.name}).model.list(value.id);
                    }
                }
            }
        }
    }

    Component {
        id: serviceDialog

        ValueDialog {
            titleText: qsTr("Service")
            model: serviceModel
            value: Settings.currentService
            onValueChanged: root.setService(value)
        }
    }

    Component {
        id: soundcloudSearchDialog

        SoundCloudSearchDialog {}
    }

    Component {
        id: pluginSearchDialog

        PluginSearchDialog {}
    }
}
