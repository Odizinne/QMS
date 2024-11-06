#include "qms.h"
#include "utils.h"
#include <QMenu>
#include <QAction>
#include <QStandardPaths>

using namespace Utils;

QMS::QMS(QWidget *parent)
    : QWidget(parent)
    , configurator(nullptr)
    , trayIcon(new QSystemTrayIcon(this))
    , settings("Odizinne", "QMS")
    , firstRun(false)
{
    loadSettings();
    createTrayIcon();

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
    screenMode = settings.value("mode", 0).toInt();
    playNotification = settings.value("notification", true).toBool();
}

void QMS::switchScreen()
{
    if (isExternalMonitorEnabled()) {
        runEnhancedDisplaySwitch(false, NULL);
        if (playNotification) {
            playSoundNotification(false);
        }
    } else {
        runEnhancedDisplaySwitch(true, screenMode);
        if (playNotification) {
            playSoundNotification(true);
        }
    }
    trayIcon->setIcon(getIcon());
}
