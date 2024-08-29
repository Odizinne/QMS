#include "utils.h"
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QProcess>

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

    if (output == "extend") {
        return true;
    }

    return false;
}

void runEnhancedDisplaySwitch(bool state)
{
    QString executablePath = "dependencies/EnhancedDisplaySwitch.exe";
    QStringList arguments;

    if (state) {
        arguments << "/extend";
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

QIcon getIcon()
{
    QString variant = isExternalMonitorEnabled() ? "secondary_" : "primary_";
    return QIcon(":/icons/icon_" + variant + getTheme() + ".png");
}
