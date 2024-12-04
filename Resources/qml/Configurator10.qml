import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Universal 2.15

ApplicationWindow {
    visible: false
    width: 360
    height: 220
    minimumWidth: 360
    maximumWidth: 360
    minimumHeight: 220
    maximumHeight: 220
    title: "QMS Settings"
    Universal.theme: Universal.System

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        anchors.leftMargin: 9
        anchors.rightMargin: 9
        anchors.topMargin: 9
        anchors.bottomMargin: 9

            GridLayout {
                id: gridLayout2
                width: parent.width

                Label {
                    id: label2
                    font.pixelSize: 13
                    font.bold: true
                    text: qsTr("Enable mode:")
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: modesComboBox
                    Layout.preferredHeight: 32
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    currentIndex: configurator.mode // Bind ComboBox to the mode property

                    // Model for the ComboBox with predefined entries
                    model: ListModel {
                        ListElement { name: "Clone" }
                        ListElement { name: "Extend" }
                        ListElement { name: "External" }
                    }

                    // Connect to the C++ function when the index changes
                    onCurrentIndexChanged: configurator.setMode(currentIndex)
                }
            }

            GridLayout {
                id: gridLayout1
                width: parent.width

                Label {
                    id: label1
                    font.pixelSize: 13
                    font.bold: true
                    text: qsTr("Play notification sound")
                    Layout.fillWidth: true
                }

                Switch {
                    id: _soundNotificationSwitch
                    Layout.preferredHeight: 32
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    checked: configurator.notification // Bind the switch to the C++ property

                    // Connect to the C++ function
                    onCheckedChanged: configurator.setNotification(checked)
                }
            }

        GridLayout {
            id: gridLayout
            Layout.fillWidth: true
            

            Label {
                id: label
                font.pixelSize: 13
                font.bold: true
                text: qsTr("Run at startup")
                Layout.fillWidth: true
            }

            Switch {
                id: _startupSwitch
                Layout.preferredHeight: 32
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                checked: configurator.runAtStartup // Bind the switch to the C++ property

                // Connect to the C++ function
                onCheckedChanged: configurator.setRunAtStartup(checked)
            }
        }
    }
}
