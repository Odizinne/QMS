#include "qms.h"
#include "utils.h"
#include <QMenu>
#include <QAction>
#include <QStandardPaths>

const QString QMS::settingsFile = QStandardPaths::writableLocation(
                                               QStandardPaths::AppDataLocation)
                                           + "/QMS/settings.json";

QMS::QMS(QWidget *parent)
    : QWidget(parent)
    , configurator(nullptr)
    , fileWatcher(new QFileSystemWatcher(this))
    , trayIcon(new QSystemTrayIcon(this))
    , firstRun(false)
{
    loadSettings();
    createTrayIcon();

    QString appDataRoaming = QDir::homePath() + "/AppData/Roaming";
    QString historyFilePath = appDataRoaming + "/EnhancedDisplaySwitch/history.txt";

    fileWatcher->addPath(historyFilePath);
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &QMS::handleFileChange);

    if (!registerGlobalHotkey()) {
        qWarning() << "Failed to register global hotkey";
    }
    if (firstRun) {
        showSettings();
    }
}

QMS::~QMS()
{
    unregisterGlobalHotkey();
    delete configurator;
    delete fileWatcher;
}

void QMS::createTrayIcon()
{
    trayIcon->setIcon(getIcon());
    QMenu *trayMenu = new QMenu(this);

    QAction *settingsAction = new QAction("Settings", this);
    connect(settingsAction, &QAction::triggered, this, &QMS::showSettings);
    trayMenu->addAction(settingsAction);

    QAction *exitAction = new QAction("Exit", this);
    connect(exitAction, &QAction::triggered, this, &QApplication::quit);
    trayMenu->addAction(exitAction);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->setToolTip("QMS");
    trayIcon->show();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &QMS::trayIconActivated);
}

void QMS::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        switchScreen();
    }
}

bool QMS::registerGlobalHotkey() {
    return RegisterHotKey((HWND)this->winId(), HOTKEY_ID, MOD_CONTROL | MOD_ALT, VK_HOME);
}

void QMS::unregisterGlobalHotkey() {
    UnregisterHotKey((HWND)this->winId(), HOTKEY_ID);
}

bool QMS::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_HOTKEY) {
        if (msg->wParam == HOTKEY_ID) {
            switchScreen();
            return true;
        }
    }
    return QWidget::nativeEvent(eventType, message, result);
}

void QMS::showSettings()
{
    if (configurator) {
        configurator->showNormal();
        configurator->raise();
        configurator->activateWindow();
        return;
    }

    configurator = new Configurator;
    configurator->setAttribute(Qt::WA_DeleteOnClose);
    connect(configurator, &Configurator::closed, this, &QMS::onConfiguratorClosed);
    configurator->show();
}

void QMS::onConfiguratorClosed()
{
    configurator = nullptr;
    loadSettings();
}

void QMS::loadSettings()
{
    QDir settingsDir(QFileInfo(settingsFile).absolutePath());
    if (!settingsDir.exists()) {
        settingsDir.mkpath(settingsDir.absolutePath());
    }

    QFile file(settingsFile);
    if (!file.exists()) {
        showSettings();

    } else {
        if (file.open(QIODevice::ReadOnly)) {
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                settings = doc.object();
                screenMode = settings.value("mode").toInt();
            }
            file.close();
        }
    }
}

void QMS::handleFileChange()
{
    trayIcon->setIcon(getIcon());

    QString appDataRoaming = QDir::homePath() + "/AppData/Roaming";
    QString historyFilePath = appDataRoaming + "/EnhancedDisplaySwitch/history.txt";

    fileWatcher->addPath(historyFilePath);
}

void QMS::switchScreen()
{
    if (isExternalMonitorEnabled()) {
        runEnhancedDisplaySwitch(false, NULL);
    } else {
        runEnhancedDisplaySwitch(true, screenMode);
    }
    trayIcon->setIcon(getIcon());
}
