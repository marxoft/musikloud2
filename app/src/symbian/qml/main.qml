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

AppWindow {
    id: appWindow

    showStatusBar: true
    showToolBar: true
    initialPage: MainPage {
        id: mainPage
    }
    
    VolumeControl {
        id: volumeControl
    }

    MyInfoBanner {
        id: infoBanner
    }

    QtObject {
        id: media

        function addTrack(track) {
            player.addTrack(track);
            infoBanner.information(qsTr("Track added to playback queue"));
        }

        function addTracks(tracks) {
            player.addTracks(tracks);
            infoBanner.information(tracks.length + " " + qsTr("Tracks added to playback queue"));
        }

        function playTrack(track, replacePage) {
            player.clearQueue();
            player.addTrack(track);

            if (replacePage) {
                appWindow.pageStack.replace(Qt.resolvedUrl("NowPlayingPage.qml"), {playWhenActive: true});
            }
            else {
                appWindow.pageStack.push(Qt.resolvedUrl("NowPlayingPage.qml"), {playWhenActive: true});
            }
        }

        function playTracks(tracks, replacePage) {
            player.clearQueue();
            player.addTracks(tracks);

            if (replacePage) {
                appWindow.pageStack.replace(Qt.resolvedUrl("NowPlayingPage.qml"), {playWhenActive: true});
            }
            else {
                appWindow.pageStack.push(Qt.resolvedUrl("NowPlayingPage.qml"), {playWhenActive: true});
            }
        }

        function addUrl(url) {
            var added = player.addUrl(url);

            if (added > 0) {
                infoBanner.information(added + " " + qsTr("Tracks added to playback queue"));
                return true;
            }

            infoBanner.information(qsTr("No tracks added to playback queue"));
            return false;
        }

        function playUrl(url, replacePage) {
            player.clearQueue();
            var added = player.addUrl(url);

            if (added > 0) {
                if (replacePage) {
                    appWindow.pageStack.replace(Qt.resolvedUrl("NowPlayingPage.qml"), {playWhenActive: true});
                }
                else {
                    appWindow.pageStack.push(Qt.resolvedUrl("NowPlayingPage.qml"), {playWhenActive: true});
                }

                return true;
            }

            infoBanner.information(qsTr("No tracks added to playback queue"));
            return false;
        }
    }

    Component.onCompleted: {
        plugins.load();
        transfers.restore();
        mainPage.setService(settings.currentService);
        
        if (settings.restorePlaybackQueueOnStartup) {
            player.restoreQueue();
        }
        
        player.error.connect(function(errorString) {
            infoBanner.information(errorString);
        });

        transfers.transferAdded.connect(function() {
            infoBanner.information(qsTr("Track added to transfers"));
        });
    }
}
