#include <helpers/appconfig.h>

#include <QApplication>
#include <coma.h>
#include <config.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Coma coma;
    coma.Helper()->initAppSettings(PROGNAME, SOFTDEVELOPER, COMAVERSION);
    app.setWindowIcon(QIcon(":/icons/coma.ico"));
    AppConfiguration::setApp(AppConfiguration::Debug);
    // Если есть аргументы запуска
    if (argc > 1)
        coma.Helper()->parseCommandLine();
    else
        coma.go();
    return app.exec();
}
