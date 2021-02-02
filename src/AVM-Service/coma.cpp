#include "coma.h"

#include "servicemodule.h"
ServiceComa::ServiceComa(QWidget *parent) : Coma(parent)
{
}

ServiceComa::~ServiceComa()
{
}

void ServiceComa::PrepareDialogs()
{
    m_Module = ModulePointer(new ServiceModule(AlarmW));
    m_Module->create(BdaTimer);
}
