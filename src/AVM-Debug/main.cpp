#include <gen/settings.h>
#include <helpers/appconfig.h>

#include <QApplication>
#include <coma.h>
#include <config.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ComaHelper *helper = new ComaHelper;
    helper->initAppSettings(PROGNAME, SOFTDEVELOPER, COMAVERSION);
    Settings::initialize();
    Coma coma;
    app.setWindowIcon(QIcon(":/icons/coma.ico"));
    AppConfiguration::setApp(AppConfiguration::Debug);
    // Если есть аргументы запуска
    if (argc > 1)
        helper->parseCommandLine();
    else
        coma.go();
    return app.exec();
}
