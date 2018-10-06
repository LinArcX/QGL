import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.4
import Squircle 1.0

Window {
    visible: true
    width: 320
    height: 480
    title: qsTr("Hello Raw OpenGl:)")

    Squircle {
    }

    Button{
        text: "clicke me"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
