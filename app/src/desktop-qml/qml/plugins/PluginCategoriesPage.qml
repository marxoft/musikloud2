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
import MusiKloud 2.0
import ".."

Page {
    id: root
    
    property alias model: categoryModel
    property alias view: view
    
    title: qsTr("Categories")
    
    TableView {
        id: view
        
        anchors.fill: parent
        focus: true
        alternatingRowColors: false
        model: PluginCategoryModel {
            id: categoryModel
            
            service: Settings.currentService
            onStatusChanged: if (status == ResourcesRequest.Failed) messageBox.showError(errorString);
        }
        itemDelegate: LabelDelegate {
            onPressed: {
                view.forceActiveFocus();
                view.selection.clear();
                view.selection.select(styleData.row);
                view.currentRow = styleData.row;
            }
            onDoubleClicked: pageStack.push({item: Qt.resolvedUrl("PluginTracksPage.qml"),
                                             properties: {title:  categoryModel.data(styleData.row, "name")}, immediate: true})
                                           .model.list(categoryModel.data(styleData.row, "value"))
        }
                
        TableViewColumn {
            role: "name"
            title: qsTr("Name")
        }
        
        Keys.onReturnPressed: if (currentRow >= 0) pageStack.push({item: Qt.resolvedUrl("PluginTracksPage.qml"),
                                                                   properties: {title:  categoryModel.data(currentRow, "name")}, immediate: true})
                                                                 .model.list(categoryModel.data(currentRow, "value"));
    }
}
