#include "iec104.h"
#include "ethernet.h"

iec104::iec104(QObject *parent) : QObject(parent)
{

}

iec104::~iec104()
{

}

void iec104::Start()
{
    APCI StartDT;
    StartDT.start = I104_START;
    StartDT.APDUlength = 4;
    StartDT.contrfield[0] = I104_STARTDT_ACT;
    StartDT.contrfield[1] = StartDT.contrfield[2] = StartDT.contrfield[3] = 0;
    void *tempp = &StartDT;
    Send(StartDT);
}

void iec104::Send(APCI apci, ASDU asdu)
{
    QByteArray ba;
    ba.resize(sizeof(apci)+sizeof(asdu));
    void *tempp = &apci;
    for (int i = 0; i < sizeof(apci);i++);
}
