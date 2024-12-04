#include "RegistryMonitor.h"

RegistryMonitor::RegistryMonitor(const QString &subKey, QObject *parent)
    : QThread(parent), m_subKey(subKey), m_stop(false) {}

RegistryMonitor::~RegistryMonitor() {
    stopMonitoring();
    wait();
}

void RegistryMonitor::stopMonitoring() {
    m_stop = true;
    if (isRunning()) {
        quit();
        wait();
    }
}

void RegistryMonitor::run() {
    HKEY hKey;
    // Open the registry key for notification
    if (RegOpenKeyEx(HKEY_CURRENT_USER, m_subKey.toStdWString().c_str(), 0, KEY_NOTIFY, &hKey) != ERROR_SUCCESS) {
        qWarning() << "Failed to open registry key for monitoring:" << m_subKey;
        return;
    }

    while (!m_stop) {
        // Wait for a registry change
        if (RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, nullptr, FALSE) == ERROR_SUCCESS) {
            emit registryChanged(); // Notify the main application
        }
    }

    RegCloseKey(hKey);
}
