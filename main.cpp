#include "QMS.h"
#include "Utils.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    if (Utils::isWindows10()) {
        a.setStyle("fusion");
    }
    QMS w;
    return a.exec();
}
