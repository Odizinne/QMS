#ifndef QMS_H
#define QMS_H

#include <QApplication>
#include <QSystemTrayIcon>
#include <windows.h>
#include <QDir>
#include <QFile>
#include <QFileSystemWatcher>
#include <QSettings>
#include "configurator.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Configurator;
}
QT_END_NAMESPACE

class QMS : public QWidget
{
    Q_OBJECT

public:
    QMS(QWidget *parent = nullptr);
    ~QMS();

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private slots:
    void showSettings();

private:
    Configurator* configurator;
    int screenMode;
    bool playNotification;
    void createTrayIcon();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    bool registerGlobalHotkey();
    void unregisterGlobalHotkey();
    QSystemTrayIcon *trayIcon;
    static const int HOTKEY_ID = 1;

    QSettings settings;
    void loadSettings();
    void onConfiguratorClosed();
    bool firstRun;
    void switchScreen();
};
#endif // QMS_H
