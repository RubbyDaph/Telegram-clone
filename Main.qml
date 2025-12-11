import QtQuick
import QtQuick.Window
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import TelegramCloneQML 1.0

Window {
    id: window

    height: 600 - 22
    minimumHeight: 600
    minimumWidth: 651
    title: qsTr("Telegrom")
    visible: true
    width: 650

    UserController {
        id: userController

    }
    RowLayout {
        id: rowLayout

        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: leftCol

            Layout.fillHeight: true
            Layout.minimumWidth: 232
            Layout.preferredWidth: 232
            color: "#141521"

            ColumnLayout {
                id: allChats

                anchors.fill: parent

                RowLayout {
                    Layout.fillWidth: true
                    Layout.maximumHeight: 79
                    Layout.minimumHeight: 79
                    Layout.preferredHeight: 79

                    Rectangle {
                        Layout.leftMargin: 10
                        Layout.maximumHeight: 20
                        Layout.maximumWidth: 20
                        Layout.minimumHeight: 20
                        Layout.minimumWidth: 20
                        Layout.preferredHeight: 20
                        Layout.preferredWidth: 20
                        color: "#1F1F2F"
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.leftMargin: 8
                        Layout.maximumHeight: 32
                        Layout.minimumHeight: 32
                        Layout.preferredHeight: 32
                        Layout.rightMargin: 16
                        color: "#434552"
                        radius: 32
                    }
                }
                Rectangle {
                    id: cpacer2

                    Layout.fillWidth: true
                    Layout.maximumHeight: 1
                    Layout.minimumHeight: 1
                    Layout.preferredHeight: 1
                    color: "#0B0B16"
                }
                ListView {
                    id: chatsList

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    focus: true
                    model: userController.chatList

                    delegate: ChatsDelegate {
                        MouseArea {
                            anchors.fill: parent

                            onClicked: {
                                userController.SetCurrentChat(model.chatId);
                                curentChat.selectedChatId = model.chatId;
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            id: cpacer1

            Layout.fillHeight: true
            Layout.maximumWidth: 1
            Layout.minimumWidth: 1
            Layout.preferredWidth: 1
            color: "#0B0B16"
        }
        Rectangle {
            id: rightCol

            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#0D0D19"

            ColumnLayout {
                id: selectedChat

                anchors.fill: parent

                Rectangle {
                    id: topChatBar

                    Layout.fillWidth: true
                    Layout.maximumHeight: 50
                    Layout.minimumHeight: 50
                    Layout.preferredHeight: 50
                    color: "#141521"

                    RowLayout {
                        anchors {
                            fill: parent
                            leftMargin: 16
                            rightMargin: 16
                        }
                        ColumnLayout {
                            Label {
                                color: "white"
                                text: "name"
                            }
                            Label {
                                color: "white"
                                text: "last seen online"
                            }
                        }
                        Rectangle {
                            id: horizontalSpacer

                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            opacity: 0
                        }
                        Rectangle {
                            id: searchIcon

                            Layout.maximumHeight: 20
                            Layout.maximumWidth: 15
                            Layout.minimumHeight: 20
                            Layout.minimumWidth: 15
                            Layout.preferredHeight: 20
                            Layout.preferredWidth: 15
                        }
                    }
                }
                ListView {
                    id: curentChat

                    property int selectedChatId: -1

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    model: selectedChatId >= 0 ? userController.chatList.getDialogModel(selectedChatId) : null
                    spacing: 5

                    delegate: DialogDelegate {
                    }
                }
                Rectangle {
                    id: bottomChatBar

                    Layout.fillWidth: true
                    Layout.minimumHeight: 42
                    Layout.preferredHeight: rowlayout.implicitHeight + 10
                    color: "#141521"

                    RowLayout {
                        id: rowlayout

                        spacing: 16

                        anchors {
                            fill: parent
                            leftMargin: 14
                            rightMargin: 7
                        }
                        Rectangle {
                            Layout.alignment: Qt.AlignBottom
                            Layout.bottomMargin: 10
                            Layout.maximumHeight: 20
                            Layout.maximumWidth: 16
                            Layout.minimumHeight: 20
                            Layout.minimumWidth: 16
                            Layout.preferredHeight: 20
                            Layout.preferredWidth: 16
                        }
                        TextInput {
                            id: messageTextInput

                            property string placeholderText: "Write a message..."

                            Layout.fillWidth: true
                            Layout.preferredHeight: implicitHeight
                            color: "white"
                            wrapMode: TextEdit.Wrap

                            Text {
                                color: "#aaa"
                                text: messageTextInput.placeholderText
                                visible: !messageTextInput.text
                            }
                        }
                        Rectangle {
                            id: emojiButton

                            Layout.alignment: Qt.AlignBottom
                            Layout.bottomMargin: 10
                            Layout.maximumHeight: 20
                            Layout.maximumWidth: 20
                            Layout.minimumHeight: 20
                            Layout.minimumWidth: 20
                            Layout.preferredHeight: 20
                            Layout.preferredWidth: 20

                            MouseArea {
                                anchors.fill: parent

                                onClicked: userController.CreateChat("Anna")
                            }
                        }
                        Rectangle {
                            id: sendButton

                            property string message: "haha"

                            Layout.alignment: Qt.AlignBottom
                            Layout.bottomMargin: 10
                            Layout.maximumHeight: 20
                            Layout.maximumWidth: 20
                            Layout.minimumHeight: 20
                            Layout.minimumWidth: 20
                            Layout.preferredHeight: 20
                            Layout.preferredWidth: 20

                            MouseArea {
                                anchors.fill: parent

                                onClicked: userController.SendMessage(messageTextInput.text)
                            }
                        }
                    }
                }
            }
        }
    }
}

