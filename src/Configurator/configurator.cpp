#include "configurator.h"
#include "ui_configurator.h"
#include "shortcutmanager.h"
#include <QStandardPaths>
#include <QDir>

using namespace ShortcutManager;
const QString Configurator::settingsFile = QStandardPaths::writableLocation(
                                      QStandardPaths::AppDataLocation)
                                  + "/QMS/settings.json";

Configurator::Configurator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Configurator)
    , firstRun(false)
{
    ui->setupUi(this);
    populateComboBox();
    loadSettings();
    initUiConnections();
    ui->startupCheckBox->setChecked(isShortcutPresent());
}

Configurator::~Configurator()
{
    saveSettings();
    emit closed();
    delete ui;
}

void Configurator::populateComboBox()
{
    ui->modeComboBox->addItem(tr("Extend"));
    ui->modeComboBox->addItem(tr("External"));
    ui->modeComboBox->addItem(tr("Clone"));
}

void Configurator::initUiConnections()
{
    connect(ui->startupCheckBox, &QCheckBox::stateChanged, this, &Configurator::manageStartupShortcut);
}

void Configurator::manageStartupShortcut()
{
    manageShortcut(ui->startupCheckBox->isChecked());
}

void Configurator::loadSettings()
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
                ui->modeComboBox->setCurrentIndex(settings.value("mode").toInt());
                ui->notificationCheckBox->setChecked(settings.value("notification").toBool());
            }
            file.close();
        }
    }
}

void Configurator::saveSettings()
{
    settings["mode"] = ui->modeComboBox->currentIndex();
    settings["notification"] = ui->notificationCheckBox->isChecked();

    QFile file(settingsFile);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(settings);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void Configurator::createDefaultSettings()
{
    firstRun = true;
    saveSettings();
}
