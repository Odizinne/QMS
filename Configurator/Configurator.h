#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QSettings>

class Configurator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool notification READ notification WRITE setNotification NOTIFY notificationChanged)
    Q_PROPERTY(bool runAtStartup READ runAtStartup WRITE setRunAtStartup NOTIFY runAtStartupChanged)

public:
    explicit Configurator(QObject *parent = nullptr);
    ~Configurator();

    int mode() const;
    bool notification() const;
    bool runAtStartup() const;
    //void setRunAtStartup(bool runAtStartup);


    void showWindow();

public slots:
    void setMode(int newMode);
    void setNotification(bool newNotification);
    void setRunAtStartup(bool newRunAtStartup);

signals:
    void modeChanged();
    void notificationChanged();
    void runAtStartupChanged();
    void settingsChanged();

private:
    QQmlApplicationEngine *engine;
    QSettings settings;
};

#endif // CONFIGURATOR_H
