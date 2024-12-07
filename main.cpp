#include "QMS.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    a.setQuitOnLastWindowClosed(false);
    QMS w;
    return a.exec();
}
