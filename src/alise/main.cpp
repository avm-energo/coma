
#include "../comaversion/comaversion.h"
#include "../gen/logger.h"
#include "../gen/stdfunc.h"
#include "controller.h"
#include "stmbroker.h"

#include <QCoreApplication>
#include <config.h>
#include <iostream>
int main(int argc, char *argv[])
{
    std::cout << "Started " << std::endl;

    GitVersion gitVersion;
    QCoreApplication a(argc, argv);
    a.setApplicationVersion(QString(COMAVERSION) + "-" + gitVersion.getGitHash());

    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addVersionOption();

    if (QCoreApplication::arguments().size() > 1)
    {
        parser.process(QCoreApplication::arguments());
        return 0;
    }
    StdFunc::Init();
    Logging::writeStart();
    qInstallMessageHandler(Logging::messageHandler);
    Controller controller;
    if (!controller.launch())
        return 13;
    std::cout << "Enter the event loop" << std::endl;
    return a.exec();
}
