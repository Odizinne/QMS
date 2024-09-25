#include "qms.h"
#include "utils.h"
#include <QApplication>

using namespace Utils;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    if (isWindows10()) {
        a.setStyle("fusion");
    }
    QMS w;
    return a.exec();
}
