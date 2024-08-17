#ifndef QMS_H
#define QMS_H

#include <QSystemTrayIcon>
#include <QMainWindow>

class QMS : public QMainWindow
{
    Q_OBJECT

public:
    QMS(QWidget *parent = nullptr);
    ~QMS();

private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    bool externalMonitorEnabled();
    void createTrayIcon();
    QSystemTrayIcon *trayIcon;
};
#endif // QMS_H
