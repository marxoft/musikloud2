/*
 * Copyright (C) 2017 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1
import QtWebKit 1.0
import com.nokia.symbian 1.1
import ".."

MyPage {
    id: root
    
    property string code

    signal accepted
    signal rejected
    
    title: qsTr("Authentication")
    showProgressIndicator: webView.status === WebView.Loading
    tools: ToolBarLayout {
        BackToolButton {
            onClicked: root.rejected()
        }
    }
    
    MyFlickable {
        id: flickable

        anchors.fill: parent
        contentWidth: webView.width
        contentHeight: webView.height
        
        WebView {
            id: webView

            preferredWidth: flickable.width
            preferredHeight: flickable.height
            settings.privateBrowsingEnabled: true
            opacity: status === WebView.Loading ? 0 : 1
            url: soundcloud.authUrl()
            onUrlChanged: {
                var s = url.toString();

                if (/code=/i.test(s)) {
                    root.code = s.split("code=")[1].split("#")[0];
                    root.accepted();
                }
            }
        }
    }
    
    ScrollDecorator {
        flickableItem: flickable
    }
    
    onAccepted: appWindow.pageStack.pop()
}
