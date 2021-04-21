#include <QCoreApplication>
#include <iostream>
#include "../interfaces/baseinterface.h"
#include "../interfaces/protocom.h"
int main(int argc, char *argv[])
{
    std::cout<<"Hello "<<std::endl;
   // BaseInterface *interface=new Proto
    BaseInterface::InterfacePointer device(new Protocom);
    device->reqBSI();
    QCoreApplication a(argc, argv);
    std::cout<<"world"<<std::endl;
    return a.exec();
}
