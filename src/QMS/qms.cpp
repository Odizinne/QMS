#include "qms.h"
#include "utils.h"
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QDebug>

QMS::QMS(QWidget *parent)
    : QMainWindow(parent)
    , trayIcon(new QSystemTrayIcon(this))
{
    createTrayIcon();
    if (!registerGlobalHotkey()) {
        qWarning() << "Failed to register global hotkey";
    }
}

QMS::~QMS() {
    unregisterGlobalHotkey();
}

void QMS::createTrayIcon()
{
    trayIcon->setIcon(getIcon());
    QMenu *trayMenu = new QMenu(this);

    QAction *exitAction = new QAction("Exit", this);
    connect(exitAction, &QAction::triggered, this, &QApplication::quit);
    trayMenu->addAction(exitAction);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &QMS::trayIconActivated);
}

void QMS::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (isExternalMonitorEnabled()) {
            runEnhancedDisplaySwitch(false);
        } else {
            runEnhancedDisplaySwitch(true);
        }
        trayIcon->setIcon(getIcon());
    }
}

bool QMS::registerGlobalHotkey() {
    return RegisterHotKey((HWND)this->winId(), HOTKEY_ID, MOD_CONTROL | MOD_ALT, VK_OEM_5);
}

void QMS::unregisterGlobalHotkey() {
    UnregisterHotKey((HWND)this->winId(), HOTKEY_ID);
}

bool QMS::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_HOTKEY) {
        if (msg->wParam == HOTKEY_ID) {
            if (isExternalMonitorEnabled()) {
                runEnhancedDisplaySwitch(false);
            } else {
                runEnhancedDisplaySwitch(true);
            }
            trayIcon->setIcon(getIcon());
            return true;
        }
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}
