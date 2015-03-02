#include "conset.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConSet w;
    w.show();

    return a.exec();
}
