#include "Configurator.h"
#include "ShortcutManager.h"
#include "Utils.h"
#include <QQmlContext>

Configurator::Configurator(QObject *parent)
    : QObject(parent)
    , settings("Odizinne", "QMS")
{
    engine = new QQmlApplicationEngine(this);
    engine->rootContext()->setContextProperty("configurator", this);

    QColor accentColor = Utils::getAccentColor("normal");
    engine->rootContext()->setContextProperty("accentColor", accentColor);

    if (Utils::isWindows10()) {
        engine->load(QUrl(QStringLiteral("qrc:/qml/Configurator10.qml")));
    } else {
        engine->load(QUrl(QStringLiteral("qrc:/qml/Configurator.qml")));
    }
}

Configurator::~Configurator()
{
    delete engine;
}

int Configurator::mode() const
{
    return settings.value("mode", 0).toInt();
}

void Configurator::setMode(int newMode)
{
    if (mode() != newMode) {
        settings.setValue("mode", newMode);
        emit modeChanged();
        emit settingsChanged();
    }
}

bool Configurator::notification() const
{
    return settings.value("notification", true).toBool();
}

void Configurator::setNotification(bool newNotification)
{
    if (notification() != newNotification) {
        settings.setValue("notification", newNotification);
        emit notificationChanged();
        emit settingsChanged();
    }
}

bool Configurator::runAtStartup() const
{
    return ShortcutManager::isShortcutPresent("QMS.lnk");
}

void Configurator::setRunAtStartup(bool newRunAtStartup)
{
    ShortcutManager::manageShortcut(newRunAtStartup, "QMS.lnk");
}

void Configurator::showWindow()
{
    QObject *rootObject = engine->rootObjects().isEmpty() ? nullptr : engine->rootObjects().first();
    if (rootObject) {
        rootObject->setProperty("visible", true);
    }
}

