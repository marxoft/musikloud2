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
    
    property variant track
    
    function list(trackData) {
        track = trackData;
        streamModel.service = track.service;
        
        if (!track.streamUrl.toString()) {
            streamModel.list(track.id);
        }
        else {
            streamModel.clear();
            streamModel.append(tr("Default format"), "");
        }
    }

    title: qsTr("Download")
    
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
            }
            
            ValueSelector {
                id: streamSelector
                
                width: parent.width
                model: PluginStreamModel {
                    id: streamModel
                    
                    onStatusChanged: {
                        switch (status) {
                        case ResourcesRequest.Loading: {
                            root.showProgressIndicator = true;
                            streamSelector.showProgressIndicator = true;
                            root.acceptable = false;
                            return;
                        }
                        case ResourcesRequest.Ready:
                            if (streamModel.count > 0) {
                                streamSelector.selectedIndex = Math.max(0, match("name", settings.defaultDownloadFormat(service)));
                                root.acceptable = true;
                            }
                            else {
                                root.acceptable = false;
                            }

                            break;
                        case ResourcesRequest.Failed: {
                            infoBanner.information(errorString);
                            root.acceptable = false;
                            break;
                        }
                        default:
                            break;
                        }
                        
                        root.showProgressIndicator = false;
                        streamSelector.showProgressIndicator = false;
                    }
                }
                title: qsTr("Audio format")
            }
            
            ValueSelector {
                id: categorySelector
                
                width: parent.width
                model: CategoryNameModel {
                    id: categoryModel
                }
                title: qsTr("Category")
                value: settings.defaultCategory
            }
        }
    }
    
    ScrollDecorator {
        flickableItem: flickable
    }
    
    onAccepted: {
        settings.setDefaultDownloadFormat(track.service,
        streamModel.data(streamSelector.selectedIndex, "name"));
        settings.defaultCategory = categorySelector.value;
        transfers.addDownloadTransfer(track.service, track.id, streamSelector.value.id, track.streamUrl,
        track.title, categorySelector.value);
        appWindow.pageStack.pop();
    }
    
    onRejected: streamModel.cancel()
}
