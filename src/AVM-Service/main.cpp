#include <QApplication>
#include <coma_core/coma.h>
#include <config.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Core::ComaHelper::initAppSettings(PROGNAME, SOFTDEVELOPER, COMAVERSION);
    app.setWindowIcon(QIcon(":/icons/coma.ico"));
    Core::Coma coma(AppConfiguration::Service);
    // Если есть аргументы запуска
    if (argc > 1)
        Core::ComaHelper::parserHelper(&coma);
    else
        coma.go();
    return app.exec();
}
