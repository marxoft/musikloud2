import QtQuick 1.1

Image {
    id: root

    property alias pressed: mouseArea.pressed

    signal clicked

    smooth: true

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        enabled: root.enabled
        onClicked: root.clicked()
    }
}
