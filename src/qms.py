import os
import sys
import json
import darkdetect
import argparse
from PyQt6.QtWidgets import QApplication, QMainWindow, QSystemTrayIcon, QMenu, QCheckBox, QLabel
from PyQt6.QtGui import QIcon
from PyQt6.QtCore import QSize, Qt
from design import Ui_MainWindow
from monitor_manager import generate_monitors, enable_monitors, disable_monitors, list_monitors
from shortcut_manager import check_startup_shortcut, manage_startup_shortcut


SETTINGS_FILE = os.path.join(os.environ["APPDATA"], "QMS", "settings.json")
ICONS_FOLDER = "icons"


class QMS(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.monitors = None
        self.init_ui()
        self.setWindowTitle("QMS - Settings")
        self.setWindowIcon(QIcon(os.path.join(ICONS_FOLDER, "icon.png")))
        self.settings = {}
        self.first_run = False
        self.secondary_monitors_enabled = self.get_active_monitors()
        self.tray_icon = self.create_tray_icon()
        self.load_settings()

    def init_ui(self):
        self.create_monitor_checkboxes()
        self.ui.startup_checkbox.setChecked(check_startup_shortcut())
        self.ui.startup_checkbox.stateChanged.connect(manage_startup_shortcut)
        self.ui.rescan_button.clicked.connect(self.create_monitor_checkboxes)

    def clear_monitor_checkboxes(self):
        while self.ui.gridLayout_2.count():
            item = self.ui.gridLayout_2.takeAt(0)
            widget = item.widget()
            if widget is not None:
                widget.deleteLater()

    def create_monitor_checkboxes(self):
        self.clear_monitor_checkboxes()
        self.monitors = generate_monitors()
        self.monitor_checkboxes = {}
        for monitor in self.monitors:
            if monitor[3] == "No":
                label = QLabel(monitor[1])
                checkbox = QCheckBox()
                checkbox.stateChanged.connect(self.save_settings)
                label.setMinimumSize(QSize(0, 25))
                checkbox.setMinimumSize(QSize(0, 25))
                checkbox.setLayoutDirection(Qt.LayoutDirection.RightToLeft)
                row = self.ui.gridLayout_2.rowCount()
                self.ui.gridLayout_2.addWidget(label, row, 0)
                self.ui.gridLayout_2.addWidget(checkbox, row, 1)
                self.monitor_checkboxes[monitor[1]] = checkbox

        self.ui.monitors_frame.adjustSize()
        self.adjustSize()
        self.resize(250, 0)
        self.resize(250, 1)
        # Disgusting but works

    def save_settings(self):
        self.settings = {
            "secondary_monitors": [
                monitor for monitor, checkbox in self.monitor_checkboxes.items() if checkbox.isChecked()
            ],
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
        for monitor, checkbox in self.monitor_checkboxes.items():
            if checkbox.isChecked():
                monitor_index = next(index for index, mon in enumerate(self.monitors) if mon[1] == monitor)
                if self.secondary_monitors_enabled:
                    disable_monitors([self.monitors[monitor_index][1]])
                else:
                    enable_monitors([self.monitors[monitor_index][1]])

        self.secondary_monitors_enabled = not self.secondary_monitors_enabled
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
    parser = argparse.ArgumentParser()
    parser.add_argument("-l", "--list", action="store_true", help="List available monitors")
    parser.add_argument("-e", "--enable", nargs="+", help="Enable secondary monitors")
    parser.add_argument("-d", "--disable", nargs="+", help="Disable secondary monitors")
    args = parser.parse_args()

    if args.list:
        list_monitors()
        sys.exit()

    elif args.enable:
        monitors = generate_monitors()
        enable_monitors(args.enable)
        sys.exit()

    elif args.disable:
        monitors = generate_monitors()
        disable_monitors(args.disable)
        sys.exit()

    else:
        app = QApplication([])
        window = QMS()
        if window.first_run:
            window.show()
        app.exec()
        sys.exit(app.exec())
