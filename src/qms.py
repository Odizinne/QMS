import os
import sys
import subprocess
import json
import xml.etree.ElementTree as ET
import darkdetect
import winshell
from PyQt6.QtWidgets import QApplication, QMainWindow, QSystemTrayIcon, QMenu
from PyQt6.QtGui import QIcon
from design import Ui_MainWindow

MULTIMONITORTOOL = os.path.join("dependencies/multimonitortool.exe")
MONITORS_XML = "dependencies/monitors.xml"
SETTINGS_FILE = os.path.join(os.environ["APPDATA"], "QMS", "settings.json")
ICONS_FOLDER = "icons"


def manage_startup_shortcut(state):
    target_path = os.path.join(os.getcwd(), "qms.exe")
    startup_folder = winshell.startup()
    shortcut_path = os.path.join(startup_folder, "qms.lnk")
    if state:
        winshell.CreateShortcut(
            Path=shortcut_path,
            Target=target_path,
            Icon=(target_path, 0),
            Description="QuickMonitorSwitcher",
            StartIn=os.path.dirname(target_path),
        )
    elif os.path.exists(shortcut_path):
        os.remove(shortcut_path)


def check_startup_shortcut():
    return os.path.exists(os.path.join(winshell.startup(), "qms.lnk"))


def create_monitors_xml():
    subprocess.run([MULTIMONITORTOOL, "/sxml", MONITORS_XML])


def parse_monitors_xml(xml_path):
    """Parse the XML file and extract monitor information."""
    tree = ET.parse(xml_path)
    root = tree.getroot()
    monitors = []

    for item in root.findall("item"):
        name = item.find("name").text
        monitor_name = item.find("monitor_name").text
        active = item.find("active").text
        monitors.append((name, monitor_name, active))

    return monitors


create_monitors_xml()

if os.path.exists(MONITORS_XML):
    monitors = parse_monitors_xml(MONITORS_XML)
    for monitor in monitors:
        print(monitor)
else:
    print("XML file does not exist.")


class QMS(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.populate_combobox()
        self.setWindowTitle("QMS - Settings")
        self.setFixedSize(self.size())
        icon = "icon_primary_light.png" if darkdetect.isDark() else "icon_primary_dark.png"
        self.setWindowIcon(QIcon(os.path.join(ICONS_FOLDER, icon)))
        self.settings = {}
        self.first_run = False
        self.secondary_monitor_enabled = self.is_selected_monitor_active()
        self.tray_icon = self.create_tray_icon()
        self.ui.comboBox.currentIndexChanged.connect(self.save_settings)
        self.ui.startup_checkbox.setChecked(check_startup_shortcut())
        self.ui.startup_checkbox.stateChanged.connect(manage_startup_shortcut)
        self.create_tray_icon()

        self.load_settings()

    def populate_combobox(self):
        for monitor in monitors:
            self.ui.comboBox.addItem(monitor[1])

    def save_settings(self):
        self.settings = {
            "secondary_monitor": self.ui.comboBox.currentText(),
        }
        os.makedirs(os.path.dirname(SETTINGS_FILE), exist_ok=True)
        with open(SETTINGS_FILE, "w") as f:
            json.dump(self.settings, f, indent=4)

    def load_settings(self):
        if os.path.exists(SETTINGS_FILE):
            with open(SETTINGS_FILE, "r") as f:
                self.settings = json.load(f)

            secondary_monitor = self.settings.get("secondary_monitor")
            if secondary_monitor:
                index = self.ui.comboBox.findText(secondary_monitor)
                if index != -1:
                    self.ui.comboBox.setCurrentIndex(index)
                else:
                    print(f"Warning: The saved monitor '{secondary_monitor}' is not in the combobox items.")
        else:
            self.first_run = True
            print("Settings file does not exist.")

    def create_tray_icon(self):
        theme = "light" if darkdetect.isDark() else "dark"
        variant = "secondary" if not self.secondary_monitor_enabled else "primary"
        tray_icon = QSystemTrayIcon(QIcon(os.path.join(ICONS_FOLDER, f"icon_{variant}_{theme}.png")))
        print(os.path.exists(os.path.join(ICONS_FOLDER, f"icon_primary_{theme}.png")))
        tray_icon.setToolTip("QMS")
        tray_icon.setContextMenu(self.create_tray_menu())
        tray_icon.show()
        return tray_icon

    def create_tray_menu(self):
        menu = QMenu()
        enable_secondary_action = (
            menu.addAction("Enable secondary monitor")
            if not self.secondary_monitor_enabled
            else menu.addAction("Disable secondary monitor")
        )
        enable_secondary_action.triggered.connect(self.toggle_secondary_monitor)
        menu.addAction(enable_secondary_action)
        settings_action = menu.addAction("Settings")
        settings_action.triggered.connect(self.show)
        menu.addAction(settings_action)
        exit_action = menu.addAction("Exit")
        exit_action.triggered.connect(QApplication.instance().quit)
        menu.addAction(exit_action)
        return menu

    def update_tray_icon(self):
        theme = "light" if darkdetect.isDark() else "dark"
        variant = "secondary" if not self.secondary_monitor_enabled else "primary"
        self.tray_icon.setIcon(QIcon(os.path.join(ICONS_FOLDER, f"icon_{variant}_{theme}.png")))

    def update_tray_menu(self):
        self.tray_icon.setContextMenu(self.create_tray_menu())

    def is_selected_monitor_active(self):
        current_monitor_name = self.ui.comboBox.currentText()
        for monitor in monitors:
            if monitor[1] == current_monitor_name:
                return monitor[2] == "Yes"
        return False

    def toggle_secondary_monitor(self):
        if self.secondary_monitor_enabled:
            displayswitch_action = "/internal"
            ddc_action = "/TurnOff"
            subprocess.run([MULTIMONITORTOOL, ddc_action, monitors[self.ui.comboBox.currentIndex()][0]])
            subprocess.run(["displayswitch.exe", displayswitch_action])
        else:
            displayswitch_action = "/extend"
            ddc_action = "/TurnOn"

            subprocess.run(["displayswitch.exe", displayswitch_action])
            subprocess.run([MULTIMONITORTOOL, ddc_action, monitors[self.ui.comboBox.currentIndex()][0]])

        self.secondary_monitor_enabled = not self.secondary_monitor_enabled
        self.update_tray_icon()
        self.update_tray_menu()

    def closeEvent(self, event):
        event.ignore()
        self.hide()


if __name__ == "__main__":
    app = QApplication([])
    window = QMS()
    if window.first_run:
        window.show()
    app.exec()
    sys.exit(app.exec())
