#include "utils.h"
#include <windows.h>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QProcess>
#include <QMediaPlayer>
#include <QAudioOutput>

bool isExternalMonitorEnabled()
{
    QString executablePath = "dependencies/EnhancedDisplaySwitch.exe";

    QProcess process;
    process.start(executablePath, QStringList() << "/lastmode");

    if (!process.waitForFinished()) {
        qDebug() << "Failed to run the command: " << process.errorString();
        return false;
    }

    QString output = process.readAllStandardOutput().trimmed();

    if (output == "internal") {
        return false;
    }

    return true;
}

void runEnhancedDisplaySwitch(bool state, int mode)
{
    QString executablePath = "dependencies/EnhancedDisplaySwitch.exe";
    QStringList arguments;

    if (state) {
        if (mode == 0) {
            arguments << "/extend";
        } else if (mode == 1) {
            arguments << "/external";
        } else if (mode == 2) {
            arguments << "/clone";
        }
    } else {
        arguments << "/internal";
    }
    QProcess process;
    process.start(executablePath, arguments);

    if (!process.waitForFinished()) {
        qDebug() << "Failed to run the command: " << process.errorString();
    }
}

QString getTheme()
{
    // Determine the theme based on registry value
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        QSettings::NativeFormat);
    int value = settings.value("AppsUseLightTheme", 1).toInt();

    // Return the opposite to match icon (dark icon on light theme)
    return (value == 0) ? "light" : "dark";
}

// Helper function to convert a BYTE value to a hex string
QString toHex(BYTE value) {
    const char* hexDigits = "0123456789ABCDEF";
    return QString("%1%2")
        .arg(hexDigits[value >> 4])
        .arg(hexDigits[value & 0xF]);
}

// Function to fetch the accent color directly from the Windows registry
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
            return QString("#%1%2%3").arg(red).arg(green).arg(blue);
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

    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            QColor pixelColor = img.pixelColor(x, y);
            if (pixelColor == QColor(255, 255, 255) || pixelColor == QColor(25, 25, 25)) {
                img.setPixelColor(x, y, color);
            }
        }
    }

    return QPixmap::fromImage(img);
}

QIcon getIcon()
{
    QString variant = isExternalMonitorEnabled() ? "secondary_" : "primary_";
    QString theme = getTheme();
    QString accentColorDark = getAccentColor("dark2");
    QString accentColorLight = getAccentColor("light3");
    QPixmap iconPixmap(":/icons/icon_" + variant + theme + ".png");

    // Get accent color based on theme and icon type
    QColor recolor;
    if (theme == "light" && variant == "secondary_") {
        recolor = QColor(getAccentColor("light3"));  // Dark 2 accent color in light theme
    } else if (theme == "dark" && variant == "secondary_") {
        qDebug() << "coucou c moi";
        qDebug() << getAccentColor("dark2");
        recolor = QColor(getAccentColor("dark2")); // Light 3 accent color in dark theme

    } else {
        recolor = (theme == "dark") ? QColor(0, 0, 0) : QColor(255, 255, 255); // Default colors
    }

    QPixmap recoloredIcon = recolorIcon(iconPixmap, recolor);

    return QIcon(recoloredIcon);
}

void playNotificationSound(QString audioFile)
{
    QMediaPlayer *player = new QMediaPlayer;

    // Create an audio output object for sound
    QAudioOutput *audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);

    // Set the media file and play it
    player->setSource(QUrl::fromLocalFile(audioFile));
    player->play();
}
