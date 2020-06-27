#include "maintray.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainTray w;
    w.show();
    return a.exec();
}
