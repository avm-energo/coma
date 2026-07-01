#include "abatcher.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icons/abatcher.ico"));

    Abatcher abatcher;
    abatcher.run();

    return app.exec();
}
