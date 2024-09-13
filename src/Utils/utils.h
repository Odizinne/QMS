#ifndef UTILS_H
#define UTILS_H

#include <QIcon>
#include <QString>

bool isExternalMonitorEnabled();
void runEnhancedDisplaySwitch(bool state, int mode);
void playNotificationSound(QString audioFile);
QIcon getIcon();

#endif // UTILS_H
