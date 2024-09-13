#include "qms.h"
#include "ui_qms.h"
#include "utils.h"
#include "shortcutmanager.h"
#include <QMenu>
#include <QAction>
#include <QStandardPaths>

const QString QMS::settingsFile = QStandardPaths::writableLocation(
                                               QStandardPaths::AppDataLocation)
                                           + "/QMS/settings.json";

QMS::QMS(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QMS)
    , fileWatcher(new QFileSystemWatcher(this))
    , trayIcon(new QSystemTrayIcon(this))
    , firstRun(false)
{
    ui->setupUi(this);
    populateComboBox();
    loadSettings();
    initUiConnections();
    createTrayIcon();
    ui->startupCheckBox->setChecked(isShortcutPresent());

    QString appDataRoaming = QDir::homePath() + "/AppData/Roaming";
    QString historyFilePath = appDataRoaming + "/EnhancedDisplaySwitch/history.txt";

    qDebug() << historyFilePath;
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
    delete fileWatcher;
    delete ui;
}

void QMS::initUiConnections()
{
    connect(ui->startupCheckBox, &QCheckBox::stateChanged, this, &QMS::manageStartupShortcut);
    connect(ui->modeComboBox, &QComboBox::currentIndexChanged, this, &QMS::saveSettings);
    connect(ui->soundCheckBox, &QCheckBox::stateChanged, this, &QMS::saveSettings);
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
            switchScreen();
            return true;
        }
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}

void QMS::manageStartupShortcut()
{
    manageShortcut(ui->startupCheckBox->isChecked());
}

void QMS::showSettings()
{
    this->show();
}

void QMS::populateComboBox()
{
    ui->modeComboBox->addItem(tr("Extend"));
    ui->modeComboBox->addItem(tr("External"));
    ui->modeComboBox->addItem(tr("Clone"));
}
void QMS::loadSettings()
{
    QDir settingsDir(QFileInfo(settingsFile).absolutePath());
    if (!settingsDir.exists()) {
        settingsDir.mkpath(settingsDir.absolutePath());
    }

    QFile file(settingsFile);
    if (!file.exists()) {
        createDefaultSettings();

    } else {
        if (file.open(QIODevice::ReadOnly)) {
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                settings = doc.object();
            }
            file.close();
        }
    }
    applySettings();
}

void QMS::createDefaultSettings()
{
    firstRun = true;
    saveSettings();
}

void QMS::applySettings()
{
    ui->modeComboBox->setCurrentIndex(settings.value("mode").toInt());
    ui->soundCheckBox->setChecked(settings.value("audioNotification").toBool());
}

void QMS::saveSettings()
{
    settings["mode"] = ui->modeComboBox->currentIndex();
    settings["audioNotification"] = ui->soundCheckBox->isChecked();

    QFile file(settingsFile);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(settings);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
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
    bool playSound = ui->soundCheckBox->isChecked();

    if (isExternalMonitorEnabled()) {
        if (playSound) {
            playNotificationSound("C:\\Windows\\Media\\Windows Hardware Remove.wav");
        }
        runEnhancedDisplaySwitch(false, NULL);
    } else {
        if (playSound) {
            playNotificationSound("C:\\Windows\\Media\\Windows Hardware Insert.wav");
        }
        runEnhancedDisplaySwitch(true, ui->modeComboBox->currentIndex());
    }
    trayIcon->setIcon(getIcon());
}
