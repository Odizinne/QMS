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

QPixmap recolorIcon(const QPixmap &originalIcon, const QColor &color, bool secondary)
{
    QImage img = originalIcon.toImage();

    if (!secondary) {
        QColor transparentColor(255, 255, 255, 0);
        for (int y = 0; y < img.height(); ++y) {
            for (int x = 0; x < img.width(); ++x) {
                QColor pixelColor = img.pixelColor(x, y);
                if (pixelColor == QColor(0, 0, 0)) {
                    img.setPixelColor(x, y, transparentColor);
                }
            }
        }
    }

    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            QColor pixelColor = img.pixelColor(x, y);
            if (pixelColor == QColor(255, 255, 255) || pixelColor == QColor(0, 0, 0)) {
                img.setPixelColor(x, y, color);
            }
        }
    }

    return QPixmap::fromImage(img);
}

QIcon getIcon()
{
    bool secondary = isExternalMonitorEnabled();
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

    QPixmap recoloredIcon = recolorIcon(iconPixmap, recolor, secondary);

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
