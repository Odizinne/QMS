#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QMainWindow>
#include <QSettings>
#include <QFile>

namespace Ui {
class Configurator;
}

class Configurator : public QMainWindow
{
    Q_OBJECT

public:
    explicit Configurator(QWidget *parent = nullptr);
    ~Configurator();

private slots:
    void manageStartupShortcut();
    void initUiConnections();

private:
    Ui::Configurator *ui;
    bool firstRun;
    QSettings settings;

    void populateComboBox();
    void loadSettings();
    void saveSettings();
    void createDefaultSettings();

signals:
    void closed();
};

#endif // CONFIGURATOR_H
