#include "qms.h"
#include "utils.h"
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QIcon>

QMS::QMS(QWidget *parent)
    : QMainWindow(parent)
    , trayIcon(new QSystemTrayIcon(this))
{
    createTrayIcon();
}

QMS::~QMS() {}

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
