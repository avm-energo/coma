#include <QApplication>
#include "coma.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Coma w;
    w.show();

    return a.exec();
}

