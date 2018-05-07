#include "pkdn_s.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("PKDN-S");
    QCoreApplication::setApplicationVersion(COMAVERSION);
    pkdn_s w;
    w.show();

    return a.exec();
}
