#ifndef UTILS_H
#define UTILS_H

#include <QIcon>

namespace Utils {

    bool isExternalMonitorEnabled();
    void runEnhancedDisplaySwitch(bool state, int mode);
    void playSoundNotification(int effect);
    QIcon getIcon();
}

#endif // UTILS_H
