#include "Utils.h"
#include "Dependencies/EnhancedDisplaySwitch/EnhancedDisplaySwitch.h"
#include <windows.h>
#include <QSettings>

QString getTheme()
{
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        QSettings::NativeFormat);
    int value = settings.value("AppsUseLightTheme", 1).toInt();

    return (value == 0) ? "light" : "dark";
}

QString toHex(BYTE value) {
    const char* hexDigits = "0123456789ABCDEF";
    return QString("%1%2")
        .arg(hexDigits[value >> 4])
        .arg(hexDigits[value & 0xF]);
}

QString getAccentColor(const QString &accentKey)
{
    HKEY hKey;
    BYTE accentPalette[32];  // AccentPalette contains 32 bytes
    DWORD bufferSize = sizeof(accentPalette);

    // Open the Windows registry key for AccentPalette
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        // Read the AccentPalette binary data
        if (RegGetValueW(hKey, NULL, L"AccentPalette", RRF_RT_REG_BINARY, NULL, accentPalette, &bufferSize) == ERROR_SUCCESS) {
            // Close the registry key after reading
            RegCloseKey(hKey);

            // Determine the correct index based on the accentKey
            int index = 0;
            if (accentKey == "dark2") index = 20;   // Index for "dark2"
            else if (accentKey == "light3") index = 0;  // Index for "light3"
            else {
                qDebug() << "Invalid accentKey provided.";
                return "#FFFFFF";  // Return white if invalid accentKey
            }

            // Extract RGB values and convert them to hex format
            QString red = toHex(accentPalette[index]);
            QString green = toHex(accentPalette[index + 1]);
            QString blue = toHex(accentPalette[index + 2]);

            // Return the hex color code
            return QString("#%1%2%3").arg(red, green, blue);
        } else {
            qDebug() << "Failed to retrieve AccentPalette from the registry.";
        }

        RegCloseKey(hKey);  // Ensure the key is closed
    } else {
        qDebug() << "Failed to open registry key.";
    }

    // Fallback color if registry access fails
    return "#FFFFFF";
}

QPixmap recolorIcon(const QPixmap &originalIcon, const QColor &color)
{
    QImage img = originalIcon.toImage();
    QColor roundedEdgesColor(color.red(), color.green(), color.blue(), color.alpha() / 3);

    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            QColor pixelColor = img.pixelColor(x, y);
            if (pixelColor == QColor(255, 255, 255) || pixelColor == QColor(0, 0, 0)) {
                img.setPixelColor(x, y, color);
            }
            if (pixelColor == QColor(127, 127, 127)) {
                img.setPixelColor(x, y, roundedEdgesColor);
            }
        }
    }

    return QPixmap::fromImage(img);
}

QIcon Utils::getIcon()
{
    std::wstring lastModeWString = EDS::getLastMode();
    QString lastMode = QString::fromStdWString(lastModeWString);
    bool secondary = (lastMode == "internal") ? false : true;

    QString theme = getTheme();
    QPixmap iconPixmap(":/icons/tray_icon.png");

    QColor recolor;
    if (theme == "light" && secondary == true) {
        recolor = QColor(getAccentColor("light3"));
    } else if (theme == "dark" && secondary == true) {
        recolor = QColor(getAccentColor("dark2"));

    } else {
        recolor = (theme == "dark") ? QColor(19, 19, 19) : QColor(255, 255, 255);
    }

    QPixmap recoloredIcon = recolorIcon(iconPixmap, recolor);

    return QIcon(recoloredIcon);
}

void Utils::playSoundNotification(int effect)
{
    const wchar_t* soundFile;

    if (effect == 1) {
        soundFile = L"C:\\Windows\\Media\\Windows Hardware Insert.wav";
    } else {
        soundFile = L"C:\\Windows\\Media\\Windows Hardware Remove.wav";
    }

    PlaySound(soundFile, NULL, SND_FILENAME | SND_ASYNC);
}

int getBuildNumber()
{
    HKEY hKey;
    RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                 TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
                 0, KEY_READ, &hKey);

    char buildNumberString[256];
    DWORD bufferSize = sizeof(buildNumberString);
    RegQueryValueEx(hKey, TEXT("CurrentBuild"), NULL, NULL, (LPBYTE)buildNumberString, &bufferSize);
    RegCloseKey(hKey);

    return std::stoi(buildNumberString);
}
