#include "../../module/module.h"
#include "testdata.h"
#include "testmodule.h"

#include <gen/stdfunc.h>

void printModuleSettings(const ModuleSettings &mSettings)
{
    auto &config = mSettings.getConfigMap();
    qDebug() << "mapSize: " << config.size();
    for (auto it = config.cbegin(); it != config.cend(); ++it)
    {
        auto &key = it.key();
        auto &val = it.value();
        qDebug() << "key: " << key << " value: " << val.size();
    }
}

void TestModule::TestS2Parsing()
{
    Modules::StartupInfoBlock bsi;
    bsi.MTypeB = 0x11;
    bsi.MTypeM = 0x11;
    bsi.Fwver = StdFunc::StrToVer(version::avma284);
    auto mModule = new Module(bsi, this);
    auto state = mModule->loadSettings();
    const auto &mSettings = ConfigStorage::GetInstance().getModuleSettings();
    printModuleSettings(mSettings);
    QCOMPARE(state, true);
}
