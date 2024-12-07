#include "RegistryMonitor.h"

RegistryMonitor::RegistryMonitor(const QString &subKey, QObject *parent)
    : QThread(parent), m_subKey(subKey), m_stop(false), m_eventHandle(CreateEvent(nullptr, TRUE, FALSE, nullptr)) {
    if (!m_eventHandle) {
        qCritical() << "Failed to create event handle for RegistryMonitor";
    }
}

RegistryMonitor::~RegistryMonitor() {
    stopMonitoring();
    if (m_eventHandle) {
        CloseHandle(m_eventHandle);
    }
}

void RegistryMonitor::stopMonitoring() {
    m_stop = true;
    if (m_eventHandle) {
        SetEvent(m_eventHandle); // Signal the thread to exit
    }
    if (isRunning()) {
        quit();
        wait(); // Wait for the thread to finish cleanly
    }
}

void RegistryMonitor::run() {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, m_subKey.toStdWString().c_str(), 0, KEY_NOTIFY, &hKey) != ERROR_SUCCESS) {
        qWarning() << "Failed to open registry key for monitoring:" << m_subKey;
        return;
    }

    while (!m_stop) {
        HANDLE handles[2] = {m_eventHandle, nullptr};
        if (hKey) {
            handles[1] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (!handles[1] || RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, handles[1], TRUE) != ERROR_SUCCESS) {
                qWarning() << "Failed to set up registry notification";
                break;
            }
        }

        DWORD result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        if (result == WAIT_OBJECT_0) { // Stop event signaled
            break;
        } else if (result == WAIT_OBJECT_0 + 1) { // Registry change event
            emit registryChanged();
        }

        if (handles[1]) {
            CloseHandle(handles[1]);
        }
    }

    RegCloseKey(hKey);
}
