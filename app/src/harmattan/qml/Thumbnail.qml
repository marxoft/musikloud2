/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

Image {
    id: root
    
    property string placeholderText
    property bool swipeEnabled: false
    
    signal clicked
    signal leftSwipe
    signal rightSwipe

    opacity: mouseArea.pressed ? UI.OPACITY_DISABLED : UI.OPACITY_ENABLED
    fillMode: Image.PreserveAspectFit
    asynchronous: true
    smooth: true
    
    Loader {
        id: loader
        
        anchors.fill: parent
        sourceComponent: (!root.source.toString()) || (root.status == Image.Error) ? placeholder : undefined
    }
    
    Component {
        id: placeholder
        
        Rectangle {
            anchors.fill: parent
            color: "#000"

            Label {
                
                function repeatedString(s) {
                    if (s.length > 0) {
                        var r = Math.floor(48 / s.length) + 1;
                        var c = Settings.activeColor;
                        var d = Qt.darker(c);
                        var t = "";
                        var u = s.toUpperCase();
                    
                        for (var i = 1; i <= r; i++) {
                            t += "<font color='" + (i % 2 ? c : d) + "'>";
                            t += u;
                            t += " </font>"
                        }
                        
                        return t;
                    }
                    
                    return s;
                }
                
                anchors.fill: parent
                clip: true
                lineHeight: Math.floor(height / 4)
                lineHeightMode: Text.FixedHeight
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WrapAnywhere
                textFormat: Text.RichText
                font {
                    bold: true
                    pixelSize: Math.floor(lineHeight / 1.3)
                }
                text: repeatedString(root.placeholderText)
            }
        }
    }

    MouseArea {
        id: mouseArea

        property int xPos

        anchors.fill: parent
        enabled: root.enabled
        onPressed: xPos = mouseX
        onReleased: {
            if (containsMouse) {
                if (root.swipeEnabled) {
                    if ((mouseX - xPos) > 100) {
                        root.rightSwipe();
                    }
                    else if ((xPos - mouseX) > 100) {
                        root.leftSwipe();
                    }
                    else {
                        root.clicked();
                    }
                }
                else {
                    root.clicked();
                }
            }
        }
    }
}
