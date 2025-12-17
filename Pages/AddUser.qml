import QtQuick
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.12
import QtQuick.Window 2.2

Dialog {
    id: addContact
    anchors.centerIn: Overlay.overlay
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    focus: true
    height: 500
    visible: false
    width: 500

    header: ToolBar{
        id: header
        RowLayout{
            id: headerLayout
            anchors.fill: parent
            Label{

                id: label
                text: "Add contact"
            }
            Button{

            }
        }

    }
    contentItem: ColumnLayout {
        id: contentLayout
        anchors.fill: parent
        TextInput {
            id: name

        }
        Button {
            id: confirmButton

        }
    }
}