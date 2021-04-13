// config.cpp
#include "config.h"

#include "../gen/board.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../widgets/checkboxgroup.h"
#include "../widgets/wd_func.h"

#include <QDebug>
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
    Parent = parent;
    QWidget *w = new QWidget;

    const auto quint16_max = std::numeric_limits<quint16>::max();
    const auto quint8_max = std::numeric_limits<quint8>::max();
    QGridLayout *glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Адрес базовой станции:"), 0, textColumn);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::Abs_104), 0, quint16_max, 0), 0, valueColumn);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Интервал циклического опроса, с:"), 1, textColumn);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::Cycle_104), 0, quint8_max, 0), 1, valueColumn);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Тайм-аут t1, с:"), 2, textColumn);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::T1_104), 0, quint8_max, 0), 2, valueColumn);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Тайм-аут t2, с:"), 3, textColumn);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::T2_104), 0, quint8_max, 0), 3, valueColumn);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Тайм-аут t3, с:"), 4, textColumn);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::T3_104), 0, quint8_max, 0), 4, valueColumn);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Макс. число неподтв. APDU (k):"), 5, textColumn);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::k_104), 0, quint8_max, 0), 5, valueColumn);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Макс. число посл. подтв. APDU (w):"), 6, textColumn);
    glyout->addWidget(WDFunc::NewSPB2(parent, nameByValue(BciNumber::w_104), 0, quint8_max, 0), 6, valueColumn);

    w->setLayout(glyout);

    return w;
}

QWidget *Config::TimeWidget(QWidget *parent)
{
    Parent = parent;
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
    auto *checkBoxGroup = new CheckBoxGroup /*<DataTypes::DWORD>*/ (ctypes, hidePositions, w);
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
    SetSPBData<DWORD>(Parent, BciNumber::Abs_104);
    SetSPBData<DWORD>(Parent, BciNumber::Cycle_104);
    SetSPBData<DWORD>(Parent, BciNumber::T1_104);
    SetSPBData<DWORD>(Parent, BciNumber::T2_104);
    SetSPBData<DWORD>(Parent, BciNumber::T3_104);
    SetSPBData<DWORD>(Parent, BciNumber::k_104);
    SetSPBData<DWORD>(Parent, BciNumber::w_104);

    CheckBoxGroup *checkBoxGroup
        = dynamic_cast<CheckBoxGroup *>(Parent->findChild<QWidget *>(nameByValue(BciNumber::CType)));
    if (checkBoxGroup)
    {
        checkBoxGroup->setBits(S2::getRecord(BciNumber::CType).value<DWORD>());
        qDebug() << checkBoxGroup->metaObject()->className();
    }
}

void Config::FillBack() const
{
    using namespace DataTypes;

    S2::setRecordValue({ BciNumber::MTypeB_ID, DWORD(Board::GetInstance().typeB()) });
    S2::setRecordValue({ BciNumber::MTypeE_ID, DWORD(Board::GetInstance().typeM()) });

    SPBDataS2<DWORD>(Parent, BciNumber::Abs_104);
    SPBDataS2<DWORD>(Parent, BciNumber::Cycle_104);
    SPBDataS2<DWORD>(Parent, BciNumber::T1_104);
    SPBDataS2<DWORD>(Parent, BciNumber::T2_104);
    SPBDataS2<DWORD>(Parent, BciNumber::T3_104);
    SPBDataS2<DWORD>(Parent, BciNumber::k_104);
    SPBDataS2<DWORD>(Parent, BciNumber::w_104);

    CheckBoxGroup *checkBoxGroup
        = dynamic_cast<CheckBoxGroup *>(Parent->findChild<QWidget *>(nameByValue(BciNumber::CType)));
    if (checkBoxGroup)
        S2::setRecordValue({ BciNumber::CType, checkBoxGroup->bits<DWORD>() });
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

QObject *Config::getParent() const
{
    return Parent;
}

void Config::setParent(QObject *value)
{
    Parent = value;
}
