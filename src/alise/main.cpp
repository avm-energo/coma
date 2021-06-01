
#include "controller.h"
#include "stmbroker.h"

#include <QCoreApplication>
#include <iostream>
int main(int argc, char *argv[])
{
    std::cout << "Started " << std::endl;

    QCoreApplication a(argc, argv);
    Controller controller;
    if (!controller.launch())
        return 13;
    std::cout << "Finished" << std::endl;
    return a.exec();
}
