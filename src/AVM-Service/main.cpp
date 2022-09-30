#include "../widgets/coma.h"

#include <QApplication>
#include <config.h>
#include <gen/logger.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ComaHelper::initAppSettings(PROGNAME, SOFTDEVELOPER, COMAVERSION);
    app.setWindowIcon(QIcon(":/icons/coma.ico"));
    Coma coma(AppConfiguration::Service);
    // Если есть аргументы запуска
    if (argc > 1)
        ComaHelper::parserHelper(&coma);
    else
        coma.go();
    return app.exec();
}
