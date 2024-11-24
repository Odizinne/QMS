#include "Configurator.h"
#include "ui_Configurator.h"
#include "ShortcutManager.h"

Configurator::Configurator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Configurator)
    , settings("Odizinne", "QMS")
{
    ui->setupUi(this);
    populateComboBox();
    loadSettings();
    initUiConnections();
    ui->startupCheckBox->setChecked(ShortcutManager::isShortcutPresent("QMS.lnk"));
}

Configurator::~Configurator()
{
    saveSettings();
    emit closed();
    delete ui;
}

void Configurator::populateComboBox()
{
    ui->modeComboBox->addItem(tr("Clone"));
    ui->modeComboBox->addItem(tr("Extend"));
    ui->modeComboBox->addItem(tr("External"));
}

void Configurator::initUiConnections()
{
    connect(ui->startupCheckBox, &QCheckBox::checkStateChanged, this, &Configurator::manageStartupShortcut);
}

void Configurator::manageStartupShortcut()
{
    ShortcutManager::manageShortcut(ui->startupCheckBox->isChecked(), "QMS.lnk");
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
