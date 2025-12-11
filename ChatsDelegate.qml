import QtQuick
import QtQuick.Layouts 1.15

Item {
    id: chatDelegate
    height: 60
    width: parent.width



    RowLayout {
        anchors.fill: parent
        Rectangle {
            Layout.preferredHeight: 44
            Layout.preferredWidth: 44
            radius: 44
        }
        ColumnLayout{
            Layout.fillWidth: true
            Text{
                id: name
                color: "white"
                text: model.contactName
            }
            Item{
                id: spacer1
                Layout.preferredHeight: 13
                Layout.minimumHeight: 13
                Layout.maximumHeight: 13
            }
            Text{
                id: lastMessage
                text: model.lastMessage
                color: "white"
            }
        }
        Rectangle{
            id: lastSeenOnline
            Layout.preferredHeight: 4
            Layout.preferredWidth: 20
            Layout.topMargin: 10
            Layout.alignment: Qt.AlignTop
        }
    }
}