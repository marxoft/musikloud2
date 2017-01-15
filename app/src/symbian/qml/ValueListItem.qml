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

MyListItem {
    id: root

    property string title: ""
    property string subTitle: ""

    flickableMode: true

    Column {
        anchors {
            verticalCenter: parent.verticalCenter
            right: indicator.left
            rightMargin: platformStyle.paddingMedium
            left: parent.left
            leftMargin: platformStyle.paddingLarge
        }

        Loader {
            anchors.left: parent.left
            sourceComponent: title != "" ? titleText : undefined
            width: parent.width // elide requires explicit width
        }

        Loader {
            anchors.left: parent.left
            sourceComponent: subTitle != "" ? subTitleText : undefined
            width: parent.width // elide requires explicit width
        }
    }

    Image {
        id: indicator
        source: root.mode == "disabled" ? privateStyle.imagePath("qtg_graf_choice_list_indicator_disabled",
                                                                 root.platformInverted)
                                        : privateStyle.imagePath("qtg_graf_choice_list_indicator",
                                                                 root.platformInverted)
        sourceSize.width: platformStyle.graphicSizeSmall
        sourceSize.height: platformStyle.graphicSizeSmall
        anchors {
            right: parent.right
            rightMargin: platformStyle.paddingSmall
            verticalCenter: parent.verticalCenter
        }
    }

    Component {
        id: titleText
        MyListItemText {
            mode: root.mode
            role: "SelectionTitle"
            text: root.title
            platformInverted: root.platformInverted
        }
    }

    Component {
        id: subTitleText
        MyListItemText {
            mode: root.mode
            role: "SelectionSubTitle"
            text: root.subTitle
            platformInverted: root.platformInverted
        }
    }
}
