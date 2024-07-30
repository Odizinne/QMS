import os
import winshell


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
