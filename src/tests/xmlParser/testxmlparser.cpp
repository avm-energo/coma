#include "../../gen/stdfunc.h"
#include "../../module/update/newmodule.h"
#include "testdata.h"
#include "testmodule.h"

void TestModule::TestS2Parsing()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0xA2;
    bsi.MTypeM = 0x84;
    bsi.Fwver = StdFunc::StrToVer(version::avma284);
    auto mModule = new NewModule(bsi, this);
    auto state = mModule->loadSettings();
    // qDebug() << "State: " << state;
    QCOMPARE(state, false);
}
