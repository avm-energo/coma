#include "abatcher.h"

#include <avm-gen/settings.h>
#include <config.h>

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Settings::initialize(PROGNAME, SOFTDEVELOPER, COMAVERSION);
    app.setWindowIcon(QIcon(":/icons/abatcher.ico"));

    Abatcher abatcher;
    abatcher.run();

    return app.exec();
}
