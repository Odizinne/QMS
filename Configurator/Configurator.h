#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QMainWindow>
#include <QSettings>

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
    QSettings settings;

    void populateComboBox();
    void loadSettings();
    void saveSettings();

signals:
    void closed();
};

#endif // CONFIGURATOR_H
