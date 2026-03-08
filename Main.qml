import QtQuick
import QtQuick.Window
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import TelegramCloneQML 1.0

Window {
    id: window

    function onNameSubmitted(textValue) {
        onClicked: userController.Login(textValue);
    }

    height: 600 - 22
    minimumHeight: 600
    minimumWidth: 651
    title: qsTr("Telegrom")
    visible: true
    width: 650

    Component.onCompleted: {
        loginLoader.active = true;
        if (loginLoader.status === Loader.Ready) {
            loginLoader.item.open();
        }
    }

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
                    spacing: 3

                    delegate: ChatsDelegate {
                        id: delegateItem

                        property bool isHovered: false

                        color: {
                            if (model.chatId === userController.currentChatId) {
                                return selectedColor;
                            } else if (isHovered) {
                                return hoveredColor;
                            } else {
                                return normalColor;
                            }
                        }
                        width: ListView.view.width

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked: {
                                userController.SetCurrentChat(model.chatId);
                                curentChat.selectedChatId = model.chatId;
                                currentContactName.selectedChatId = model.chatId;
                            }
                            onEntered: delegateItem.isHovered = true
                            onExited: delegateItem.isHovered = false
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
                                id: currentContactName

                                property string selectedChatId: ""

                                color: "white"
                                text: selectedChatId != "" ? userController.chatList.getContactNameByID(selectedChatId) : ""
                            }
                            Label {
                                color: "white"
                                text: currentContactName.selectedChatId == "" ? "last seen online" : ""
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

                    property string selectedChatId: " "

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    model: selectedChatId != " " ? userController.chatList.getDialogModel(selectedChatId) : null
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

                                onClicked: {
                                    onClicked: userController.CreateChat("Kirill");
                                }
                            }
                        }
                        Image {
                            id: sendButton

                            Layout.rightMargin: 5
                            Layout.alignment: Qt.AlignBottom
                            Layout.bottomMargin: 10
                            Layout.maximumHeight: 20
                            Layout.maximumWidth: 20
                            Layout.minimumHeight: 20
                            Layout.minimumWidth: 20
                            Layout.preferredHeight: 20
                            Layout.preferredWidth: 20
                            fillMode: Image.PreserveAspectCrop
                            source: "qrc:/icons/sendArrow.png"

                            MouseArea {
                                anchors.fill: parent

                                onClicked: {
                                    userController.SendMessage(messageTextInput.text);
                                    messageTextInput.clear();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Loader {
        id: loginLoader

        active: false
        source: "Pages/login.qml"

        onLoaded: {
            item.parent = window.contentItem;
            item.closed.connect(function () {
                loginLoader.active = false;
            });
            item.nameSubmitted.connect(onNameSubmitted);
        }
    }
}
