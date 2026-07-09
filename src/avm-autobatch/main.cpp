#include "abatcher.h"
#include <libavm-gen/settings.h>

#include <QApplication>
#include <QIcon>
#include <config.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Settings::initialize(PROGNAME, SOFTDEVELOPER, COMAVERSION);
    app.setWindowIcon(QIcon(":/icons/abatcher.ico"));

    Abatcher abatcher;
    abatcher.run();

    return app.exec();
}
