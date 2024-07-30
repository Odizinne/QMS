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
            (item.find("name").text, item.find("monitor_name").text, item.find("active").text)
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
        self.setFixedSize(self.size())
        self.setWindowIcon(QIcon(os.path.join(ICONS_FOLDER, "icon.png")))
        self.settings = {}
        self.first_run = False
        self.secondary_monitor_enabled = self.is_selected_monitor_active()
        self.tray_icon = self.create_tray_icon()
        self.load_settings()

    def init_ui(self):
        for monitor in self.monitors:
            self.ui.comboBox.addItem(monitor[1])
        self.ui.comboBox.currentIndexChanged.connect(self.save_settings)
        self.ui.startup_checkbox.setChecked(check_startup_shortcut())
        self.ui.startup_checkbox.stateChanged.connect(manage_startup_shortcut)

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
                    pass
        else:
            self.first_run = True

    def create_tray_icon(self):
        theme = "light" if darkdetect.isDark() else "dark"
        variant = "secondary" if not self.secondary_monitor_enabled else "primary"
        tray_icon = QSystemTrayIcon(QIcon(os.path.join(ICONS_FOLDER, f"icon_{variant}_{theme}.png")))
        tray_icon.setToolTip("QMS")
        tray_icon.setContextMenu(self.create_tray_menu())
        tray_icon.activated.connect(self.handle_tray_icon_click)
        tray_icon.show()
        return tray_icon

    def handle_tray_icon_click(self, reason):
        if reason == QSystemTrayIcon.ActivationReason.Trigger:
            self.toggle_secondary_monitor()

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
        exit_action.triggered.connect(self.exit_app)
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
        for monitor in self.monitors:
            if monitor[1] == current_monitor_name:
                return monitor[2] == "Yes"
        return False

    def toggle_secondary_monitor(self):
        if self.secondary_monitor_enabled:
            displayswitch_action = "/internal"
            ddc_action = "/TurnOff"
            subprocess.run([MULTIMONITORTOOL, ddc_action, self.monitors[self.ui.comboBox.currentIndex()][0]])
            subprocess.run(["displayswitch.exe", displayswitch_action])
        else:
            displayswitch_action = "/extend"
            ddc_action = "/TurnOn"

            subprocess.run(["displayswitch.exe", displayswitch_action])
            subprocess.run([MULTIMONITORTOOL, ddc_action, self.monitors[self.ui.comboBox.currentIndex()][0]])

        self.secondary_monitor_enabled = not self.secondary_monitor_enabled
        self.update_tray_icon()
        self.update_tray_menu()

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
