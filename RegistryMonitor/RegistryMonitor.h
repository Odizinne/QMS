#ifndef REGISTRYMONITOR_H
#define REGISTRYMONITOR_H

#include <QThread>
#include <QObject>
#include <windows.h>
#include <QString>
#include <QDebug>

class RegistryMonitor : public QThread {
    Q_OBJECT

public:
    explicit RegistryMonitor(const QString &subKey, QObject *parent = nullptr);
    ~RegistryMonitor();

    void stopMonitoring();

signals:
    void registryChanged();

protected:
    void run() override;

private:
    QString m_subKey;
    bool m_stop;
    HANDLE m_eventHandle; // Event handle for signaling
};

#endif // REGISTRYMONITOR_H
