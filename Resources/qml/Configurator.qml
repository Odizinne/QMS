import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.FluentWinUI3 2.15

ApplicationWindow {
    visible: false
    width: 360
    height: 220
    minimumWidth: 360
    maximumWidth: 360
    minimumHeight: 220
    maximumHeight: 220
    title: "QMS Settings"

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        anchors.leftMargin: 9
        anchors.rightMargin: 9
        anchors.topMargin: 9
        anchors.bottomMargin: 9

        Frame {
            id: frame2
            Layout.fillWidth: true

            GridLayout {
                id: gridLayout2
                anchors.fill: parent

                Label {
                    id: label2
                    font.pixelSize: 13
                    font.bold: true
                    text: qsTr("Enable mode:")
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
        }

        Frame {
            id: frame1
            Layout.fillWidth: true

            GridLayout {
                id: gridLayout1
                anchors.fill: parent

                Label {
                    id: label1
                    font.pixelSize: 13
                    font.bold: true
                    text: qsTr("Play notification sound")
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
        }

        Frame {
            id: frame
            Layout.fillWidth: true

            GridLayout {
                id: gridLayout
                anchors.fill: parent

                Label {
                    id: label
                    font.pixelSize: 13
                    font.bold: true
                    text: qsTr("Run at startup")
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
}
