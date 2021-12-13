#include "../gen/logger.h"
#include "servicecoma.h"

#include <QApplication>
#include <config.h>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QCoreApplication::setApplicationName(PROGNAME);
    QCoreApplication::setOrganizationName(SOFTDEVELOPER);
    QCoreApplication::setApplicationVersion(COMAVERSION);
    ComaHelper::initResources();
    a.setWindowIcon(QIcon(":/icons/coma.ico"));
    Logging::writeStart();
    qInstallMessageHandler(Logging::messageHandler);
    ServiceComa w;
    // есть аргументы запуска
    if (argc > 1)
    {
        ComaHelper::parserHelper(PROGNAME, &w);
    }
    else
    {
        w.go();
    }

    return a.exec();
}
