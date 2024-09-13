#ifndef QMS_H
#define QMS_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <windows.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>
#include <QFile>
#include <QFileSystemWatcher>

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

private slots:
    void manageStartupShortcut();
    void showSettings();

private:
    Ui::QMS *ui;
    void initUiConnections();
    void createTrayIcon();
    QFileSystemWatcher *fileWatcher;
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    bool registerGlobalHotkey();
    void unregisterGlobalHotkey();
    QSystemTrayIcon *trayIcon;
    static const int HOTKEY_ID = 1;

    QString settingsFilePath;
    QJsonObject settings;
    static const QString settingsFile;
    void loadSettings();
    void createDefaultSettings();
    void applySettings();
    void saveSettings();
    bool firstRun;
    void populateComboBox();
    void handleFileChange();
    void switchScreen();
};
#endif // QMS_H
