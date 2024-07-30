import os
import sys
import subprocess
import json
import xml.etree.ElementTree as ET
import darkdetect
import winshell
from PyQt6.QtWidgets import QApplication, QMainWindow, QSystemTrayIcon, QMenu, QCheckBox
from PyQt6.QtGui import QIcon
from design import Ui_MainWindow


MULTIMONITORTOOL = os.path.join("dependencies", "multimonitortool.exe")
MONITORS_XML = os.path.join("dependencies", "monitors.xml")
SETTINGS_FILE = os.path.join(os.environ["APPDATA"], "QMS", "settings.json")
ICONS_FOLDER = "icons"
STARTUP_SHORTCUT_NAME = "qms.lnk"
TARGET_PATH = os.path.join(os.getcwd(), "qms.exe")


def manage_startup_shortcut(state):
    startup_folder = winshell.startup()
    shortcut_path = os.path.join(startup_folder, STARTUP_SHORTCUT_NAME)

    if state:
        winshell.CreateShortcut(
            Path=shortcut_path,
            Target=TARGET_PATH,
            Icon=(TARGET_PATH, 0),
            Description="QuickMonitorSwitcher",
            StartIn=os.path.dirname(TARGET_PATH),
        )
    elif os.path.exists(shortcut_path):
        os.remove(shortcut_path)


def check_startup_shortcut():
    return os.path.exists(os.path.join(winshell.startup(), STARTUP_SHORTCUT_NAME))


def create_monitors_xml():
    try:
        subprocess.run([MULTIMONITORTOOL, "/sxml", MONITORS_XML], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error creating monitors XML: {e}")
        sys.exit(1)


def parse_monitors_xml(xml_path):
    """Parse the XML file and extract monitor information."""
    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()
        monitors = [
            (
                item.find("name").text,
                item.find("monitor_name").text,
                item.find("active").text,
                item.find("primary").text,
            )
            for item in root.findall("item")
        ]
        return monitors
    except ET.ParseError as e:
        print(f"Error parsing monitors XML: {e}")
        sys.exit(1)


def generate_monitors():
    create_monitors_xml()
    return parse_monitors_xml(MONITORS_XML)


class QMS(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.monitors = generate_monitors()
        self.init_ui()
        self.setWindowTitle("QMS - Settings")
        self.setWindowIcon(QIcon(os.path.join(ICONS_FOLDER, "icon.png")))
        self.settings = {}
        self.first_run = False
        self.secondary_monitors_enabled = self.get_active_monitors()
        self.tray_icon = self.create_tray_icon()
        self.load_settings()

    def init_ui(self):
        self.monitor_checkboxes = {}
        for monitor in self.monitors:
            if monitor[3] == "No":
                checkbox = QCheckBox(monitor[1])
                checkbox.stateChanged.connect(self.save_settings)
                self.ui.gridLayout_2.addWidget(checkbox)
                self.monitor_checkboxes[monitor[1]] = checkbox
        self.adjustSize()
        self.setFixedSize(250, self.height())
        self.ui.startup_checkbox.setChecked(check_startup_shortcut())
        self.ui.startup_checkbox.stateChanged.connect(manage_startup_shortcut)

    def save_settings(self):
        self.settings = {
            "secondary_monitors": [
                monitor for monitor, checkbox in self.monitor_checkboxes.items() if checkbox.isChecked()
            ]
        }
        os.makedirs(os.path.dirname(SETTINGS_FILE), exist_ok=True)
        with open(SETTINGS_FILE, "w") as f:
            json.dump(self.settings, f, indent=4)

    def load_settings(self):
        if os.path.exists(SETTINGS_FILE):
            with open(SETTINGS_FILE, "r") as f:
                self.settings = json.load(f)

            for monitor, checkbox in self.monitor_checkboxes.items():
                checkbox.setChecked(monitor in self.settings.get("secondary_monitors", []))
        else:
            self.first_run = True

    def create_tray_icon(self):
        theme = "light" if darkdetect.isDark() else "dark"
        variant = "secondary" if not self.secondary_monitors_enabled else "primary"
        tray_icon = QSystemTrayIcon(QIcon(os.path.join(ICONS_FOLDER, f"icon_{variant}_{theme}.png")))
        tray_icon.setToolTip("QMS")
        tray_icon.setContextMenu(self.create_tray_menu())
        tray_icon.activated.connect(self.handle_tray_icon_click)
        tray_icon.show()
        return tray_icon

    def handle_tray_icon_click(self, reason):
        if reason == QSystemTrayIcon.ActivationReason.Trigger:
            self.toggle_secondary_monitors()

    def create_tray_menu(self):
        menu = QMenu()
        enable_secondary_action = (
            menu.addAction("Enable secondary monitors")
            if not self.secondary_monitors_enabled
            else menu.addAction("Disable secondary monitors")
        )
        enable_secondary_action.triggered.connect(self.toggle_secondary_monitors)
        menu.addAction(enable_secondary_action)
        settings_action = menu.addAction("Settings")
        settings_action.triggered.connect(self.show)
        menu.addAction(settings_action)
        exit_action = menu.addAction("Exit")
        exit_action.triggered.connect(self.exit_app)
        menu.addAction(exit_action)
        return menu

    def update_tray_icon(self):
        theme = "light" if darkdetect.isDark() else "dark"
        variant = "secondary" if not self.secondary_monitors_enabled else "primary"
        self.tray_icon.setIcon(QIcon(os.path.join(ICONS_FOLDER, f"icon_{variant}_{theme}.png")))

    def update_tray_menu(self):
        self.tray_icon.setContextMenu(self.create_tray_menu())

    def get_active_monitors(self):
        active_monitors_count = sum(1 for monitor in self.monitors if monitor[2] == "Yes" and monitor[3] == "No")
        return active_monitors_count > 0

    def toggle_secondary_monitors(self):
        if self.secondary_monitors_enabled:
            self.disable_secondary_monitors()
            self.secondary_monitors_enabled = False
        else:
            self.enable_secondary_monitors()
            self.secondary_monitors_enabled = True

        self.update_tray_icon()
        self.update_tray_menu()

    def enable_secondary_monitors(self):
        # Run multimonitortool before displayswitch
        subprocess.run(["displayswitch.exe", "/extend"])
        for monitor, checkbox in self.monitor_checkboxes.items():
            if checkbox.isChecked():
                monitor_index = next(index for index, mon in enumerate(self.monitors) if mon[1] == monitor)
                if not self.secondary_monitors_enabled:
                    subprocess.run([MULTIMONITORTOOL, "/TurnOn", self.monitors[monitor_index][0]])

    def disable_secondary_monitors(self):
        # Run displayswitch after multimonitortool
        for monitor, checkbox in self.monitor_checkboxes.items():
            if checkbox.isChecked():
                monitor_index = next(index for index, mon in enumerate(self.monitors) if mon[1] == monitor)
                if self.secondary_monitors_enabled:
                    subprocess.run([MULTIMONITORTOOL, "/TurnOff", self.monitors[monitor_index][0]])

        subprocess.run(["displayswitch.exe", "/internal"])

    def exit_app(self):
        self.close()
        self.tray_icon.hide()
        QApplication.quit()
        sys.exit()

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
