// config.cpp
#include "config.h"

#include "../gen/board.h"
#include "../gen/s2.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QMetaEnum>
#include <QVBoxLayout>

Config::Config()
{
    // Q_ASSERT(sizeof(Bci::BciMain) / 4 == regs.size());
    MainBlk.MTypeB = Board::GetInstance().typeB();
    MainBlk.MTypeM = Board::GetInstance().typeM();
    setConfig();
}

void Config::setConfig()
{
    removeFotter();

    // общая часть
    //    for (const auto &item:regs)
    //    {

    //        config->append({item})
    //    }
    auto &config = S2::config;
    //    auto iter = regs.cbegin();
    //    while (iter != regs.end())
    //    {
    //        config->append({ quint32(iter.key()), sizeof(quint32), iter.value() });
    //        ++iter;
    //    }
    // общая часть

    config.append({ BciNumber::MTypeB_ID, sizeof(MainBlk.MTypeB), &MainBlk.MTypeB });
    config.append({ BciNumber::MTypeE_ID, sizeof(MainBlk.MTypeM), &MainBlk.MTypeM });
    config.append({ BciNumber::CType, sizeof(MainBlk.Ctype), &MainBlk.Ctype });
    config.append({ BciNumber::Abs_104, sizeof(MainBlk.Abs_104), &MainBlk.Abs_104 });
    config.append({ BciNumber::Cycle_104, sizeof(MainBlk.Cycle_104), &MainBlk.Cycle_104 });
    config.append({ BciNumber::T1_104, sizeof(MainBlk.T1_104), &MainBlk.T1_104 });
    config.append({ BciNumber::T2_104, sizeof(MainBlk.T2_104), &MainBlk.T2_104 });
    config.append({ BciNumber::T3_104, sizeof(MainBlk.T3_104), &MainBlk.T3_104 });
    config.append({ BciNumber::k_104, sizeof(MainBlk.K_104), &MainBlk.K_104 });
    config.append({ BciNumber::w_104, sizeof(MainBlk.W_104), &MainBlk.W_104 });
    config.append({ 0xFFFFFFFF, 0, nullptr });
}

void Config::SetDefConf()
{
    MainBlk = Bci::BciMain();
    // Не нужно, всё описано в конструкторе
    //    auto defValues = QMetaEnum::fromType<Bci::BciDefMainValues>();
    //    int i = 0;

    //    std::for_each(regs.begin(), regs.end(), [&](quint32 *value) { *value = defValues.value(i++); });
}

QWidget *Config::MainWidget(QWidget *parent)
{
    ParentMainbl = parent;
    QWidget *w = new QWidget;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    glyout->setColumnStretch(2, 50);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Адрес базовой станции:"), 0, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB2(parent, NAMEOF(MainBlk.Abs_104), 0, 65535, 0), 0, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Интервал циклического опроса, с:"), 1, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB2(parent, NAMEOF(MainBlk.Cycle_104), 0, 255, 0), 1, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Тайм-аут t1, с:"), 2, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB2(parent, NAMEOF(MainBlk.T1_104), 0, 255, 0), 2, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Тайм-аут t2, с:"), 3, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB2(parent, NAMEOF(MainBlk.T2_104), 0, 255, 0), 3, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Тайм-аут t3, с:"), 4, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB2(parent, NAMEOF(MainBlk.T3_104), 0, 255, 0), 4, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Макс. число неподтв. APDU (k):"), 5, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB2(parent, NAMEOF(MainBlk.K_104), 0, 255, 0), 5, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL2(parent, "Макс. число посл. подтв. APDU (w):"), 6, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB2(parent, NAMEOF(MainBlk.W_104), 0, 255, 0), 6, 1, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);

    w->setLayout(vlyout2);

    return w;
}

QWidget *Config::TimeWidget(QWidget *parent)
{
    ParentCtype = parent;
    QWidget *w = new QWidget;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    glyout->setColumnStretch(2, 50);
    int row = 0;
    QLabel *lbl = new QLabel("Тип синхронизации времени:");

    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);

    const QStringList cbl { "нет", "SNTP", "SNTP+PPS" };
    glyout->addWidget(WDFunc::NewCB2(parent, NAMEOF(MainBlk.Ctype), cbl), row, 1, 1, 1);

    vlyout2->addLayout(glyout);

    w->setLayout(vlyout2);

    return w;
}

void Config::Fill()
{

    WDFunc::SetSPBData(ParentMainbl, NAMEOF(MainBlk.Abs_104), MainBlk.Abs_104);
    WDFunc::SetSPBData(ParentMainbl, NAMEOF(MainBlk.Cycle_104), MainBlk.Cycle_104);
    WDFunc::SetSPBData(ParentMainbl, NAMEOF(MainBlk.T1_104), MainBlk.T1_104);
    WDFunc::SetSPBData(ParentMainbl, NAMEOF(MainBlk.T2_104), MainBlk.T2_104);
    WDFunc::SetSPBData(ParentMainbl, NAMEOF(MainBlk.T3_104), MainBlk.T3_104);
    WDFunc::SetSPBData(ParentMainbl, NAMEOF(MainBlk.K_104), MainBlk.K_104);
    WDFunc::SetSPBData(ParentMainbl, NAMEOF(MainBlk.W_104), MainBlk.W_104);
    // Miss first 3 value
    //    int i = 3;
    //    auto defValues = QMetaEnum::fromType<Bci::BciDefMainValues>();
    //    std::for_each(regs.begin() + 3, regs.end(), [&](quint32 *value) {
    //        if (!WDFunc::SetSPBData(ParentMainbl, defValues.key(i++), *value))
    //            qDebug() << "Failed to find" << defValues.key(i);
    //    });
    int cbidx;
    switch (MainBlk.Ctype)
    {
    case 0:
        cbidx = 0;
        break;
    case 8:
        cbidx = 1;
        break;
    case 10:
        cbidx = 2;
        break;
    default:
        return;
    }
    WDFunc::SetCBIndex(ParentCtype, NAMEOF(MainBlk.Ctype), cbidx);
}

void Config::FillBack()
{
    //    int i = 3;
    //    auto defValues = QMetaEnum::fromType<Bci::BciDefMainValues>();
    //    std::for_each(regs.begin() + 3, regs.end(),
    //        [&](quint32 *value) { WDFunc::SPBData(ParentMainbl, defValues.key(i++), *value); });

    WDFunc::SPBData(ParentMainbl, NAMEOF(MainBlk.Abs_104), MainBlk.Abs_104);
    WDFunc::SPBData(ParentMainbl, NAMEOF(MainBlk.Cycle_104), MainBlk.Cycle_104);
    WDFunc::SPBData(ParentMainbl, NAMEOF(MainBlk.T1_104), MainBlk.T1_104);
    WDFunc::SPBData(ParentMainbl, NAMEOF(MainBlk.T2_104), MainBlk.T2_104);
    WDFunc::SPBData(ParentMainbl, NAMEOF(MainBlk.T3_104), MainBlk.T3_104);
    WDFunc::SPBData(ParentMainbl, NAMEOF(MainBlk.K_104), MainBlk.K_104);
    WDFunc::SPBData(ParentMainbl, NAMEOF(MainBlk.W_104), MainBlk.W_104);

    int cbidx = WDFunc::CBIndex(ParentCtype, NAMEOF(MainBlk.Ctype));
    switch (cbidx)
    {
    case 0:
        MainBlk.Ctype = 0;
        break;
    case 1:
        MainBlk.Ctype = 8;
        break;
    case 2:
        MainBlk.Ctype = 10;
        break;
    }
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
