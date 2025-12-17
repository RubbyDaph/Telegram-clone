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
            clip: true
            Layout.preferredHeight: 44
            Layout.preferredWidth: 44
            radius: 44
            color: chatDelegate.normalColor
            Rectangle{
                id: head
                opacity: 1
                anchors.centerIn: parent
                anchors.bottomMargin: 5
                height: 20
                width: 20
                radius: 10

            }
            Rectangle{
                id: body
                opacity: 1
                width: 30
                radius: 10
                height: 30
                x: 7
                y: width + 2
            }
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