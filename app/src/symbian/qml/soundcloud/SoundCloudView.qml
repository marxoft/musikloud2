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
        model: SoundCloudNavModel {
            id: navModel
        }
        delegate: TextDelegate {
            subItemIndicator: true
            text: display
            onClicked: {
                switch (index) {
                case 0:
                    appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudAccountsPage.qml"));
                    break;
                case 1:
                    appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudSearchPage.qml"));
                    break;
                case 2:
                    appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudTracksPage.qml"), {title: display})
                    .get("/me/tracks", {limit: MAX_RESULTS});
                    break;
                case 3:
                    appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudTracksPage.qml"), {title: display})
                    .get("/me/favorites", {limit: MAX_RESULTS});
                    break;
                case 4:
                    appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudPlaylistsPage.qml"), {title: display})
                    .get("/me/playlists", {limit: MAX_RESULTS});
                    break;
                case 5:
                    appWindow.pageStack.push(Qt.resolvedUrl("SoundCloudArtistsPage.qml"), {title: display})
                    .get("/me/followings", {limit: MAX_RESULTS});
                    break;
                default:
                    break;
                }
            }
        }
    }
    
    Connections {
        target: soundcloud
        onCommentAdded: infoBanner.information(qsTr("Your comment has been added"))
        onArtistFollowed: infoBanner.information(qsTr("You have followed") + " " + artist.name)
        onArtistUnfollowed: infoBanner.information(qsTr("You have unfollowed") + " " + artist.name)
        onTrackFavourited: infoBanner.information("'" + track.title + "' " + qsTr("added to favourites"))
        onTrackUnfavourited: infoBanner.information("'" + track.title + "' " + qsTr("removed from favourites"))
    }    
}
