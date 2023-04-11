#include "sysjournal.h"

SysJournal::SysJournal(QObject *parent) : BaseJournal(parent)
{
    jourName = "системный журнал";
    viewName = "system";
    headers = AVM::eventJourHeaders;
    description = AVM::sysJourDescription;
}

void SysJournal::fillTable(const QByteArray &ba)
{
    Q_UNUSED(ba);
}
