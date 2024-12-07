#ifndef UTILS_H
#define UTILS_H

#include <QIcon>

namespace Utils {
    bool isExternalMonitorEnabled();
    void runEnhancedDisplaySwitch(bool state, int mode);
    void playSoundNotification(int effect);
    QIcon getIcon();
    bool isWindows10();
    QString getAccentColor(const QString &accentKey);
}

#endif // UTILS_H
