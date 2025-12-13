import QtQuick
import QtQuick.Layouts 1.15

Rectangle {
    id: chatDelegate

    property color hoveredColor: "#1C1D2A"
    property bool isHovered: false
    property bool isSelected: false
    property color normalColor: "#141521"
    property color selectedColor: "#2B305D"

    color: normalColor
    height: 60
    width: parent.width

    RowLayout {
        anchors.fill: parent

        Rectangle {
            Layout.preferredHeight: 44
            Layout.preferredWidth: 44
            radius: 44
        }
        ColumnLayout {
            Layout.fillWidth: true

            Text {
                id: name

                color: "white"
                text: model.contactName
            }
            Item {
                id: spacer1

                Layout.maximumHeight: 13
                Layout.minimumHeight: 13
                Layout.preferredHeight: 13
            }
            Text {
                id: lastMessage

                color: "white"
                text: model.lastMessage
            }
        }
        Rectangle {
            id: lastSeenOnline

            Layout.alignment: Qt.AlignTop
            Layout.preferredHeight: 4
            Layout.preferredWidth: 20
            Layout.topMargin: 10
        }
    }
}