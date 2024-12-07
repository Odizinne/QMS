import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Universal 2.15

ApplicationWindow {
    visible: false
    //width:
    //height:
    minimumWidth: 300
    minimumHeight: 200
    maximumWidth: 300
    maximumHeight: 200

    title: "QMS Settings"
    Universal.theme: Universal.System
    Universal.accent: accentColor

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        anchors.topMargin: 15
        anchors.bottomMargin: 15

            ColumnLayout {
                id: gridLayout2
                //columns: 1

                Label {
                    id: label2
                    font.pixelSize: 13
                    text: qsTr("Enable mode:")
                    Layout.fillWidth: true

                }

                ComboBox {
                    id: modesComboBox
                    Layout.fillWidth: true


                    Layout.preferredHeight: 32
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
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

            ColumnLayout {
                id: gridLayout1
                Layout.fillWidth: true

                Label {
                    id: label1
                    font.pixelSize: 13
                    text: qsTr("Play notification sound")
                    Layout.fillWidth: true


                }

                Switch {
                    id: _soundNotificationSwitch
                    Layout.leftMargin: -8
                    Layout.preferredHeight: 32
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    checked: configurator.notification // Bind the switch to the C++ property

                    // Connect to the C++ function
                    onCheckedChanged: configurator.setNotification(checked)
                }
            }

        ColumnLayout {
            id: gridLayout
            Layout.fillWidth: true
            Label {
                id: label
                font.pixelSize: 13
                text: qsTr("Run at startup")
                Layout.fillWidth: true


            }

            Switch {
                id: _startupSwitch
                Layout.preferredHeight: 32
                Layout.leftMargin: -8
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                checked: configurator.runAtStartup // Bind the switch to the C++ property

                // Connect to the C++ function
                onCheckedChanged: configurator.setRunAtStartup(checked)
            }
        }
    }
}
