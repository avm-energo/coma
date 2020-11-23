// config.cpp
#include "config.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../widgets/wd_func.h"

#include <QVBoxLayout>
Config::Config(S2DataTypes::S2ConfigType *config)
{
    //    MainBlk.MTypeB = MTypeB;
    //    MainBlk.MTypeM = MTypeM;
    MainBlk.MTypeB = Board::GetInstance().typeB();
    MainBlk.MTypeM = Board::GetInstance().typeM();
    setConfig(config);
}

void Config::setConfig(S2DataTypes::S2ConfigType *config)
{
    config->erase(
        std::remove_if(config->begin(), config->end(), [](S2DataTypes::DataRec i) { return i.id == 0xFFFFFFFF; }),
        config->end());
    // общая часть
    config->append({ BCI_MTYPEB, sizeof(MainBlk.MTypeB), &MainBlk.MTypeB });
    config->append({ BCI_MTYPEM, sizeof(MainBlk.MTypeM), &MainBlk.MTypeM });
    config->append({ BCI_CTYPE, sizeof(MainBlk.Ctype), &MainBlk.Ctype });
    config->append({ BCI_ABS_104, sizeof(MainBlk.Abs_104), &MainBlk.Abs_104 });
    config->append({ BCI_CYCLE_104, sizeof(MainBlk.Cycle_104), &MainBlk.Cycle_104 });
    config->append({ BCI_T1_104, sizeof(MainBlk.T1_104), &MainBlk.T1_104 });
    config->append({ BCI_T2_104, sizeof(MainBlk.T2_104), &MainBlk.T2_104 });
    config->append({ BCI_T3_104, sizeof(MainBlk.T3_104), &MainBlk.T3_104 });
    config->append({ BCI_K_104, sizeof(MainBlk.k_104), &MainBlk.k_104 });
    config->append({ BCI_W_104, sizeof(MainBlk.w_104), &MainBlk.w_104 });
    config->append({ 0xFFFFFFFF, 0, nullptr });
}

void Config::SetDefConf()
{
    MainBlk.MTypeB = DEF_MTYPEB;
    MainBlk.MTypeM = DEF_MTYPEM;
    MainBlk.Ctype = DEF_CTYPE;
    MainBlk.Abs_104 = DEF_ABS_104;
    MainBlk.Cycle_104 = DEF_CYCLE_104;
    MainBlk.T1_104 = DEF_T1_104;
    MainBlk.T2_104 = DEF_T2_104;
    MainBlk.T3_104 = DEF_T3_104;
    MainBlk.k_104 = DEF_K_104;
    MainBlk.w_104 = DEF_W_104;
}

QWidget *Config::MainWidget(QWidget *parent)
{
    ParentMainbl = parent;
    QWidget *w = new QWidget;
    QString paramcolor = Colors::MAINWINCLR;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    glyout->setColumnStretch(2, 50);

    glyout->addWidget(WDFunc::NewLBL(parent, "Адрес базовой станции:"), 0, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB(parent, "Abs_104", 0, 65535, 0, paramcolor), 0, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL(parent, "Интервал циклического опроса, с:"), 1, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB(parent, "Cycle_104", 0, 255, 0, paramcolor), 1, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL(parent, "Тайм-аут t1, с:"), 2, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB(parent, "T1_104", 0, 255, 0, paramcolor), 2, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL(parent, "Тайм-аут t2, с:"), 3, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB(parent, "T2_104", 0, 255, 0, paramcolor), 3, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL(parent, "Тайм-аут t3, с:"), 4, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB(parent, "T3_104", 0, 255, 0, paramcolor), 4, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL(parent, "Макс. число неподтв. APDU (k):"), 5, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB(parent, "k_104", 0, 255, 0, paramcolor), 5, 1, 1, 1, Qt::AlignLeft);

    glyout->addWidget(WDFunc::NewLBL(parent, "Макс. число посл. подтв. APDU (w):"), 6, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB(parent, "w_104", 0, 255, 0, paramcolor), 6, 1, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);

    w->setLayout(vlyout2);
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}");
    return w;
}

QWidget *Config::TimeWidget(QWidget *parent)
{
    ParentCtype = parent;
    QString paramcolor = Colors::MAINWINCLR;
    QWidget *w = new QWidget;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    QFont font;
    font.setFamily("Times");
    font.setPointSize(11);

    glyout->setColumnStretch(2, 50);
    int row = 0;
    QLabel *lbl;

    lbl = new QLabel("Тип синхронизации времени:");

    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);

    QStringList cbl = { "нет", "SNTP", "SNTP+PPS" };
    glyout->addWidget(WDFunc::NewCB(parent, "Ctype", cbl, paramcolor), row, 1, 1, 1);

    vlyout2->addLayout(glyout);

    w->setLayout(vlyout2);
    w->setStyleSheet("QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}");
    return w;
}

void Config::Fill()
{
    int cbidx;
    WDFunc::SetSPBData(ParentMainbl, "Abs_104", MainBlk.Abs_104);
    WDFunc::SetSPBData(ParentMainbl, "Cycle_104", MainBlk.Cycle_104);
    WDFunc::SetSPBData(ParentMainbl, "T1_104", MainBlk.T1_104);
    WDFunc::SetSPBData(ParentMainbl, "T2_104", MainBlk.T2_104);
    WDFunc::SetSPBData(ParentMainbl, "T3_104", MainBlk.T3_104);
    WDFunc::SetSPBData(ParentMainbl, "k_104", MainBlk.k_104);
    WDFunc::SetSPBData(ParentMainbl, "w_104", MainBlk.w_104);

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
    WDFunc::SetCBIndex(ParentCtype, "Ctype", cbidx);
}

void Config::FillBack()
{
    int cbidx;
    WDFunc::SPBData(ParentMainbl, "Abs_104", MainBlk.Abs_104);
    WDFunc::SPBData(ParentMainbl, "Cycle_104", MainBlk.Cycle_104);
    WDFunc::SPBData(ParentMainbl, "T1_104", MainBlk.T1_104);
    WDFunc::SPBData(ParentMainbl, "T2_104", MainBlk.T2_104);
    WDFunc::SPBData(ParentMainbl, "T3_104", MainBlk.T3_104);
    WDFunc::SPBData(ParentMainbl, "k_104", MainBlk.k_104);
    WDFunc::SPBData(ParentMainbl, "w_104", MainBlk.w_104);

    cbidx = WDFunc::CBIndex(ParentCtype, "Ctype");
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
