#ifndef QMS_H
#define QMS_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <windows.h>

class QMS : public QMainWindow
{
    Q_OBJECT

public:
    QMS(QWidget *parent = nullptr);
    ~QMS();

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    void createTrayIcon();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    bool registerGlobalHotkey();
    void unregisterGlobalHotkey();

    QSystemTrayIcon *trayIcon;
    static const int HOTKEY_ID = 1; // ID for the hotkey
};

#endif // QMS_H
