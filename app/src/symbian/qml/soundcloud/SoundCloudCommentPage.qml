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
import QSoundCloud 1.0 as QSoundCloud
import ".."

TextInputPage {
    id: root

    property string trackId
    
    title: qsTr("Add comment")
    label: qsTr("Comment")
    multiLine: true
    
    SoundCloudComment {
        id: comment
        
        onStatusChanged: {
            switch (status) {
            case QSoundCloud.ResourcesRequest.Loading:
                root.showProgressIndicator = true;
                root.acceptable = false;
                return;
            case QSoundCloud.ResourcesRequest.Ready:
                appWindow.pageStack.pop();
                break;
            case QSoundCloud.ResourcesRequest.Failed:
                infoBanner.information(errorString);
                break;
            default:
                break;
            }
            
            root.showProgressIndicator = false;
            root.acceptable = (root.text != "");
        }
    }
    
    onAccepted: {
        var data = {};
        data["track_id"] = trackId;
        data["body"] = text;
        comment.addComment(data);
    }
    
    onRejected: comment.cancel()
}
