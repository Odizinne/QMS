#include "utils.h"
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

QIcon getIcon()
{
    QString variant = isExternalMonitorEnabled() ? "secondary_" : "primary_";
    return QIcon(":/icons/icon_" + variant + getTheme() + ".png");
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
