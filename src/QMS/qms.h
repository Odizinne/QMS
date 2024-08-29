#ifndef QMS_H
#define QMS_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <windows.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class QMS;
}
QT_END_NAMESPACE

class QMS : public QMainWindow
{
    Q_OBJECT

public:
    QMS(QWidget *parent = nullptr);
    ~QMS();

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    Ui::QMS *ui;
    void createTrayIcon();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    bool registerGlobalHotkey();
    void unregisterGlobalHotkey();

    QSystemTrayIcon *trayIcon;
    static const int HOTKEY_ID = 1;
};
#endif // QMS_H
