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
import QSoundCloud 1.0 as QSoundCloud
import ".."

MyDialog {
    id: root
        
    property string resourceId
    property string resourceTitle
        
    minimumWidth: grid.width + 20
    minimumHeight: grid.height + 60
    title: qsTr("Download track")
    content: GridLayout {
        id: grid
        
        columns: 2
        
        Label {
            text: qsTr("Audio format") + ":"
        }
        
        ComboBox {
            id: streamSelector
        
            Layout.minimumWidth: 200
            model: SoundCloudStreamModel {
                id: streamModel
            
                onStatusChanged: {
                    switch (status) {
                    case QSoundCloud.StreamsRequest.Ready:
                        if (count > 0) {
                            streamSelector.currentIndex = Math.max(0, match("name",
                                                                   Settings.defaultDownloadFormat(Resources.SOUNDCLOUD)));
                        }
                        else {
                            messageBox.showError(qsTr("No streams found"));
                        }

                        break;
                    case QSoundCloud.StreamsRequest.Failed: {
                        messageBox.showError(errorString);
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
            textRole: "name"
            onActivated: Settings.setDefaultDownloadFormat(Resources.SOUNDCLOUD, streamModel.data(index, "name"))
        }        
        
        Label {
            text: qsTr("Category") + ":"
        }
        
        ComboBox {
            id: categorySelector
            
            Layout.minimumWidth: 200
            model: CategoryNameModel {
                id: categoryModel
            }
            textRole: "name"
            currentIndex: categoryModel.match("value", Settings.defaultCategory)
            onActivated: Settings.defaultCategory = categoryModel.data(index, "value")
        }
    }
    buttons: [
        Button {
            text: qsTr("&Cancel")
            iconName: "dialog-cancel"
            onClicked: root.reject()
        },
        
        Button {
            text: qsTr("&Ok")
            iconName: "dialog-ok"
            isDefault: true
            enabled: (streamModel.status == QSoundCloud.ResourcesRequest.Ready) && (streamModel.count > 0)
            onClicked: root.accept()
        }
    ]
    
    onOpened: streamModel.get(resourceId)
    onRejected: streamModel.cancel()
    onAccepted: Transfers.addDownloadTransfer(Resources.SOUNDCLOUD, resourceId,
                                              streamModel.data(streamSelector.currentIndex, "value").id, "",
                                              resourceTitle, categoryModel.data(categorySelector.currentIndex, "value"))
}
