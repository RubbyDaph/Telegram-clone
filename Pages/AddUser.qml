import QtQuick
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.12
import QtQuick.Window 2.2

Dialog {
    id: addContact

    property color dialogBackground: '#141521'
    property alias contactName: nameInput.text
    signal nameSubmitted(string name)
    anchors.centerIn: Overlay.overlay
    closePolicy: Popup.CloseOnEscape
    focus: true
    height: 450
    modal: true
    visible: false
    width: 450

    Overlay.modal: Rectangle {
        color: "black"
        opacity: 0.5
    }
    background: Rectangle {
        anchors.fill: parent
        color: addContact.dialogBackground
    }
    contentItem: ColumnLayout {
        id: contentLayout

        anchors.fill: parent

        Item {
            id: contentSpacer1

            Layout.fillHeight: true
        }
        Rectangle {
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            Layout.preferredHeight: 30
            Layout.preferredWidth: 120
            color: "#2A2C3B"
            TextInput {
                id: nameInput

                property string placeholderText: "Name"
                maximumLength: 12
                anchors.fill: parent
                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter
                color: "white"
                Text {
                    anchors.fill: parent
                    color: "#aaa"
                    text: nameInput.placeholderText
                    visible: !nameInput.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        Button {
            id: confirmButton

            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
            Layout.preferredHeight: 35
            Layout.preferredWidth: 120
            property string textHolder: "Confirm"

            background: Rectangle {
                id: buttonBackground

                color: confirmButton.hovered ? "#444863" : "#2A2C3B"
            }
            contentItem: Text {
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                text: confirmButton.textHolder
                verticalAlignment: Text.AlignVCenter
            }
            onClicked:{
                    addContact.nameSubmitted(nameInput.text)
                    nameInput.clear()
                    addContact.close()
            }
        }
        Item {
            id: contentSpacer2

            Layout.fillHeight: true
        }
    }
    header: ToolBar {
        id: header

        height: 25

        background: Rectangle {
            anchors.fill: parent
            color: addContact.dialogBackground
        }

        RowLayout {
            id: headerLayout

            anchors.fill: parent

            Item {
                id: headerSpacer1

                Layout.fillWidth: true
            }
            Label {
                id: label

                Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
                color: "white"
                text: "Add contact"
            }
            Item {
                id: headerSpacer2

                Layout.fillWidth: true
            }
            Button {
                id: closeButton
                onClicked: addContact.close()
                Layout.preferredHeight: 20
                Layout.preferredWidth: 20

                background: Rectangle {
                    anchors.fill: parent
                    color: closeButton.hovered ? "red" : addContact.dialogBackground
                }

                Rectangle {
                    id: closeButtonIcon1

                    height: 15
                    width: 2
                    x: 8
                    y: 2.5

                    transform: Rotation {
                        angle: 45
                        origin.x: 1
                        origin.y: 15 / 2
                    }
                }
                Rectangle {
                    id: closeButtonIcon2

                    height: 15
                    width: 2
                    x: 8
                    y: 2.5

                    transform: Rotation {
                        angle: 45 + 90
                        origin.x: 1
                        origin.y: 15 / 2
                    }
                }
            }
        }
    }
}
