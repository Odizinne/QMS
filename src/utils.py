import platform
import winreg


def is_windows_10():
    os_release = platform.release()
    os_name = platform.system()
    return os_name == "Windows" and os_release == "10"


def is_dark_mode_enabled():
    registry_key = winreg.OpenKey(
        winreg.HKEY_CURRENT_USER, r"Software\Microsoft\Windows\CurrentVersion\Themes\Personalize"
    )
    value, regtype = winreg.QueryValueEx(registry_key, "AppsUseLightTheme")
    winreg.CloseKey(registry_key)
    return value == 0
