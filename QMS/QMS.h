#ifndef QMS_H
#define QMS_H

#include <QApplication>
#include <QSystemTrayIcon>
#include <Windows.h>
#include <QSettings>
#include "Configurator.h"
#include "RegistryMonitor.h"

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
    void onRegistryChanged();

private:
    Configurator *configurator;
    RegistryMonitor *registryMonitor;
    QSystemTrayIcon *trayIcon;

    QSettings settings;

    int screenMode;
    bool playNotification;
    void createTrayIcon();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    bool registerGlobalHotkey();
    void unregisterGlobalHotkey();
    static const int HOTKEY_ID = 1;

    void loadSettings();
    void onConfiguratorClosed();
    void switchScreen();
};
#endif // QMS_H
