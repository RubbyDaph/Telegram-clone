import QtQuick
import QtQuick.Layouts 1.15

Rectangle {
    id: message
    implicitWidth: content.width + 20
    implicitHeight: content.height + 10
    color: "#161727"
    radius: 20
    ColumnLayout {
        anchors.fill: parent
        Text {
            id: content
            Layout.alignment: Qt.AlignHCenter

            Layout.topMargin: 5
            color: "white"
            text: model.content
        }
        Text {
            id: timestamp
            Layout.alignment: Qt.AlignBottom | Qt.AlignRight
            Layout.bottomMargin: 5
            Layout.rightMargin: 5
            text: model.timestamp
        }
    }
}