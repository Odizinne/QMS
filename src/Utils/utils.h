#ifndef UTILS_H
#define UTILS_H

#include <QIcon>
#include <QString>

namespace Utils {

    bool isExternalMonitorEnabled();
    void runEnhancedDisplaySwitch(bool state, int mode);
    void playSoundNotification(bool enabled);
    QIcon getIcon();
}

#endif // UTILS_H
