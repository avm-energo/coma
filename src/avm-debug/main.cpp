#include <common/appconfig.h>
#include <gen/settings.h>

#include <QApplication>
#include <coma.h>
#include <config.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ComaHelper *helper = new ComaHelper;
    AppConfiguration::setApp(AppConfiguration::Debug);
    helper->initAppSettings();
    Settings::initialize(PROGNAME, SOFTDEVELOPER, COMAVERSION);
    app.setWindowIcon(QIcon(":/icons/coma.ico"));
    Coma coma;
    // Если есть аргументы запуска
    if (argc > 1)
        helper->parseCommandLine();
    else
        coma.go();
    return app.exec();
}
