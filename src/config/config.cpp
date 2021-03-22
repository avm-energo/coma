// config.cpp
#include "config.h"

#include "../gen/board.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../widgets/checkboxgroup.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QMetaEnum>
#include <QVBoxLayout>

Config::Config()
{
    using namespace DataTypes;
}

void Config::SetDefConf()
{
    using namespace DataTypes;
    S2::setRecordValue({ BciNumber::MTypeB_ID, DWORD(Board::GetInstance().typeB()) });
    S2::setRecordValue({ BciNumber::MTypeE_ID, DWORD(Board::GetInstance().typeM()) });
    S2::setRecordValue({ BciNumber::Abs_104, DWORD(Bci::DEF_ABS_104) });
    S2::setRecordValue({ BciNumber::Cycle_104, DWORD(Bci::DEF_CYCLE_104) });
    S2::setRecordValue({ BciNumber::T1_104, DWORD(Bci::DEF_T1_104) });
    S2::setRecordValue({ BciNumber::T2_104, DWORD(Bci::DEF_T2_104) });
    S2::setRecordValue({ BciNumber::T3_104, DWORD(Bci::DEF_T3_104) });
    S2::setRecordValue({ BciNumber::k_104, DWORD(Bci::DEF_K_104) });
    S2::setRecordValue({ BciNumber::w_104, DWORD(Bci::DEF_W_104) });
    S2::setRecordValue({ BciNumber::CType, DWORD(Bci::DEF_CTYPE) });
}

QWidget *Config::MainWidget(QWidget *parent)
{
    ParentMainbl = parent;
    QWidget *w = new QWidget;

    QGridLayout *glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Адрес базовой станции:"), 0, 0);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::Abs_104), 0, 65535, 0), 0, 1);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Интервал циклического опроса, с:"), 1, 0);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::Cycle_104), 0, 255, 0), 1, 1);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Тайм-аут t1, с:"), 2, 0);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::T1_104), 0, 255, 0), 2, 1);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Тайм-аут t2, с:"), 3, 0);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::T2_104), 0, 255, 0), 3, 1);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Тайм-аут t3, с:"), 4, 0);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::T3_104), 0, 255, 0), 4, 1);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Макс. число неподтв. APDU (k):"), 5, 0);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::k_104), 0, 255, 0), 5, 1);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Макс. число посл. подтв. APDU (w):"), 6, 0);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::w_104), 0, 255, 0), 6, 1);

    w->setLayout(glyout);

    return w;
}

QWidget *Config::TimeWidget(QWidget *parent)
{
    ParentCtype = parent;
    QWidget *w = new QWidget;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    glyout->setColumnStretch(2, 50);

    vlyout2->addLayout(glyout);

    const QStringList ctypes { "SNTP  IP:172.16.28.2", "синхронизация по входу 1PPS", "SNTP  IP:172.16.28.1",
        "адрес SNTP   берется из конфигурации" };
    auto deviceType = Board::GetInstance().deviceType();
    QList<int> hidePositions;
    switch (deviceType)
    {
    case Board::Controller:
        hidePositions = { PPS, SNTP_Default };
        break;
    case Board::Module:
        hidePositions = { SNTP_Addr1, SNTP_Addr2 };
        break;
    default:
        hidePositions = {};
    }
    auto *checkBoxGroup = new CheckBoxGroup<DataTypes::DWORD>(ctypes, hidePositions, w);
    checkBoxGroup->setObjectName("ctype");

    vlyout2->addWidget(checkBoxGroup);

    w->setLayout(vlyout2);

    return w;
}

void Config::Fill()
{
    using namespace DataTypes;
    const auto s2typeB = S2::getRecord(BciNumber::MTypeB_ID).value<DWORD>();
    if (s2typeB != Board::GetInstance().typeB())
        qCritical() << "Conflict typeB, module: " << QString::number(Board::GetInstance().typeB(), 16)
                    << " config: " << QString::number(s2typeB, 16);
    const auto s2typeM = S2::getRecord(BciNumber::MTypeE_ID).value<DWORD>();
    if (s2typeM != Board::GetInstance().typeM())
        qCritical() << "Conflict typeB, module: " << QString::number(Board::GetInstance().typeM(), 16)
                    << " config: " << QString::number(s2typeM, 16);
    SetSPBData<DWORD>(ParentMainbl, BciNumber::Abs_104);
    SetSPBData<DWORD>(ParentMainbl, BciNumber::Cycle_104);
    SetSPBData<DWORD>(ParentMainbl, BciNumber::T1_104);
    SetSPBData<DWORD>(ParentMainbl, BciNumber::T2_104);
    SetSPBData<DWORD>(ParentMainbl, BciNumber::T3_104);
    SetSPBData<DWORD>(ParentMainbl, BciNumber::k_104);
    SetSPBData<DWORD>(ParentMainbl, BciNumber::w_104);

    CheckBoxGroup<DataTypes::DWORD> *checkBoxGroup
        = dynamic_cast<CheckBoxGroup<DataTypes::DWORD> *>(ParentMainbl->findChild<QWidget *>("ctype"));
    if (checkBoxGroup)
        checkBoxGroup->setBits(S2::getRecord(BciNumber::CType).value<DWORD>());
}

void Config::FillBack() const
{
    using namespace DataTypes;

    S2::setRecordValue({ BciNumber::MTypeB_ID, DWORD(Board::GetInstance().typeB()) });
    S2::setRecordValue({ BciNumber::MTypeE_ID, DWORD(Board::GetInstance().typeM()) });

    SPBDataS2<DWORD>(ParentMainbl, BciNumber::Abs_104);
    SPBDataS2<DWORD>(ParentMainbl, BciNumber::Cycle_104);
    SPBDataS2<DWORD>(ParentMainbl, BciNumber::T1_104);
    SPBDataS2<DWORD>(ParentMainbl, BciNumber::T2_104);
    SPBDataS2<DWORD>(ParentMainbl, BciNumber::T3_104);
    SPBDataS2<DWORD>(ParentMainbl, BciNumber::k_104);
    SPBDataS2<DWORD>(ParentMainbl, BciNumber::w_104);

    CheckBoxGroup<DataTypes::DWORD> *checkBoxGroup
        = dynamic_cast<CheckBoxGroup<DataTypes::DWORD> *>(ParentMainbl->findChild<QWidget *>("ctype"));
    if (checkBoxGroup)
        S2::setRecordValue({ BciNumber::CType, checkBoxGroup->bits() });
}

void Config::removeFotter()
{
    quint64 counter = 0;
    S2::config.erase(std::remove_if(S2::config.begin(), S2::config.end(),
                         [&](S2DataTypes::DataRec i) {
                             if (i.id == 0xFFFFFFFF)
                                 counter++;
                             return i.id == 0xFFFFFFFF;
                         }),
        S2::config.end());
    Q_ASSERT(counter < 2);
}
