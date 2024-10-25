#include "configurator.h"
#include "ui_configurator.h"
#include "shortcutmanager.h"
#include <QStandardPaths>
#include <QDir>

using namespace ShortcutManager;

Configurator::Configurator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Configurator)
    , firstRun(false)
    , settings("Odizinne", "QMS")
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
    ui->modeComboBox->setCurrentIndex(settings.value("mode", 0).toInt());
    ui->notificationCheckBox->setChecked(settings.value("notification", true).toBool());
}

void Configurator::saveSettings()
{
    settings.setValue("mode", ui->modeComboBox->currentIndex());
    settings.setValue("notification", ui->notificationCheckBox->isChecked());
}

void Configurator::createDefaultSettings()
{
    firstRun = true;
    saveSettings();
}
