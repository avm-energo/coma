#include "pkdn_s.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    pkdn_s w;
    w.show();

    return a.exec();
}
