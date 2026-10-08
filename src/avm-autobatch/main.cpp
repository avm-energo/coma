#include "abatcher.h"
#include <common/appconfig.h>
#include <libavm-gen/settings.h>

#include <QApplication>
#include <QIcon>
#include <config.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(PROGNAME);
    app.setApplicationVersion(COMAVERSION);
    app.setWindowIcon(QIcon(":/icons/abatcher.ico"));

    AppConfiguration::setApp(AppConfiguration::Service);
    Settings::setConfigDirOverride(":/settings/");

    Abatcher abatcher;
    abatcher.run();

    return app.exec();
}
