
#include "../gen/logger.h"
#include "controller.h"
#include "stmbroker.h"

#include <QCoreApplication>
#include <iostream>
int main(int argc, char *argv[])
{
    std::cout << "Started " << std::endl;

    QCoreApplication a(argc, argv);
    //  Logging::writeStart();
    // qInstallMessageHandler(Logging::messageHandler);
    Controller controller;
    if (!controller.launch())
        return 13;
    std::cout << "Enter the event loop" << std::endl;
    return a.exec();
}
