#include "coma.h"

#include "tunemodule.h"

DebugComa::DebugComa(QWidget *parent) : Coma(parent)
{
}

DebugComa::~DebugComa()
{
}

void DebugComa::PrepareDialogs()
{
    m_Module = ModulePointer(new TuneModule(AlarmW));
    m_Module->create(BdaTimer);
}
