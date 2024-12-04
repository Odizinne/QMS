#include "QMS.h"
#include <QApplication>
#include "Utils.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    QMS w;
    return a.exec();
}
