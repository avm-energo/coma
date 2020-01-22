#include <QGroupBox>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QTime>
#include <QTimer>
#include "../widgets/emessagebox.h"
#include "../widgets/ecombobox.h"
#include "../widgets/wd_func.h"
//#include "../gen/publicclass.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "confdialog84.h"
#include "../gen/commands.h"
#include "../gen/error.h"


ConfDialog84::ConfDialog84(QVector<S2::DataRec> &S2Config, QWidget *parent) :
    AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    this->S2Config = &S2Config;
    C84 = new Config84(S2Config);
     setAttribute(Qt::WA_DeleteOnClose);

    SetupUI();
#if PROGSIZE != PROGSIZE_EMUL
    PrereadConf();
#endif
}

ConfDialog84::~ConfDialog84()
{
}

void ConfDialog84::Fill()
{
    int i;
    WDFunc::SetSPBData(this, "Unom", C84->Bci_block.Unom);
    WDFunc::SetSPBData(this, "Umin", C84->Bci_block.Umin);
    WDFunc::SetSPBData(this, "Imin", C84->Bci_block.Imin);

    for (i = 0; i < 3; i++)
    {
        //WDFunc::SetSPBData(this, "Imax."+QString::number(i), C84->Bci_block.Imax[i]);
        //WDFunc::SetSPBData(this, "C_init."+QString::number(i), C84->Bci_block.C_init[i]);
        //WDFunc::SetSPBData(this, "Tg_init."+QString::number(i), C84->Bci_block.Tg_init[i]);
        //WDFunc::SetSPBData(this, "corTg."+QString::number(i), C84->Bci_block.corTg[i]);
          WDFunc::SetSPBData(this, "Tg_pasp."+QString::number(i), C84->Bci_block.Tg_pasp[i]);
          WDFunc::SetSPBData(this, "C_pasp."+QString::number(i), C84->Bci_block.C_pasp[i]);
    }

    WDFunc::SetSPBData(this, "dС_pred", C84->Bci_block.dC_pred);
    WDFunc::SetSPBData(this, "dС_alarm", C84->Bci_block.dC_alarm);
    WDFunc::SetSPBData(this, "dTg_pred", C84->Bci_block.dTg_pred);
    WDFunc::SetSPBData(this, "dTg_alarm", C84->Bci_block.dTg_alarm);
    WDFunc::SetSPBData(this, "dIunb_pred", C84->Bci_block.dIunb_pred);
    WDFunc::SetSPBData(this, "dIunb_alarm", C84->Bci_block.dIunb_alarm);
    WDFunc::SetSPBData(this, "GdС", C84->Bci_block.GdC);
    WDFunc::SetSPBData(this, "GdTg", C84->Bci_block.GdTg);
    WDFunc::SetSPBData(this, "GdIunb", C84->Bci_block.GdIunb);
    WDFunc::SetSPBData(this, "NFiltr", C84->Bci_block.NFiltr);
    WDFunc::SetSPBData(this, "T_Data_Rec", C84->Bci_block.T_Data_Rec);
    WDFunc::SetSPBData(this, "Phy_unb_init", C84->Bci_block.Phy_unb_init);
    WDFunc::SetSPBData(this, "Iunb_init", C84->Bci_block.Iunb_init);
    WDFunc::SetSPBData(this, "Unom_1", C84->Bci_block.Unom_1);

    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    {
        WDFunc::SetSPBData(this, "RTerm", C84->Bci_block.RTerm);
        WDFunc::SetSPBData(this, "W100", C84->Bci_block.W100);

        for (i = 0; i < 4; i++)
        {
             WDFunc::SetSPBData(this, "IP"+QString::number(i), C84->Com_param.IP[i]);
             WDFunc::SetSPBData(this, "Mask"+QString::number(i), C84->Com_param.Mask[i]);
             WDFunc::SetSPBData(this, "GateWay"+QString::number(i), C84->Com_param.GateWay[i]);
             WDFunc::SetSPBData(this, "Port"+QString::number(i), C84->Com_param.Port[i]);
             WDFunc::SetSPBData(this, "SNTP"+QString::number(i), C84->Com_param.SNTP[i]);
        }

        WDFunc::SetSPBData(this, "Baud", C84->Com_param.baud);
        WDFunc::SetSPBData(this, "Parity", C84->Com_param.parity);
        WDFunc::SetSPBData(this, "StopBit", C84->Com_param.stopbit);
        WDFunc::SetSPBData(this, "adrMB", C84->Com_param.adrMB);

        if(C84->Com_param.isNTP)
        WDFunc::SetChBData(this, "ISNTP", true);
        else
        WDFunc::SetChBData(this, "ISNTP", false);
    }

}

void ConfDialog84::FillBack()
{
    int i;
    QString tmps;

    WDFunc::SPBData(this, "Unom", C84->Bci_block.Unom);
    WDFunc::SPBData(this, "Umin", C84->Bci_block.Umin);
    WDFunc::SPBData(this, "Imin", C84->Bci_block.Imin);

    for (i = 0; i < 3; i++)
    {
        //WDFunc::SPBData(this, "Imax."+QString::number(i), C84->Bci_block.Imax[i]);
        //WDFunc::SPBData(this, "C_init."+QString::number(i), C84->Bci_block.C_init[i]);
        //WDFunc::SPBData(this, "Tg_init."+QString::number(i), C84->Bci_block.Tg_init[i]);
        //WDFunc::SPBData(this, "corTg."+QString::number(i), C84->Bci_block.corTg[i]);
        WDFunc::SPBData(this, "Tg_pasp."+QString::number(i), C84->Bci_block.Tg_pasp[i]);
        WDFunc::SPBData(this, "C_pasp."+QString::number(i), C84->Bci_block.C_pasp[i]);
    }

    WDFunc::SPBData(this, "dС_pred", C84->Bci_block.dC_pred);
    WDFunc::SPBData(this, "dС_alarm", C84->Bci_block.dC_alarm);
    WDFunc::SPBData(this, "dTg_pred", C84->Bci_block.dTg_pred);
    WDFunc::SPBData(this, "dTg_alarm", C84->Bci_block.dTg_alarm);
    WDFunc::SPBData(this, "dIunb_pred", C84->Bci_block.dIunb_pred);
    WDFunc::SPBData(this, "dIunb_alarm", C84->Bci_block.dIunb_alarm);
    WDFunc::SPBData(this, "GdС", C84->Bci_block.GdC);
    WDFunc::SPBData(this, "GdTg", C84->Bci_block.GdTg);
    WDFunc::SPBData(this, "GdIunb", C84->Bci_block.GdIunb);
    WDFunc::SPBData(this, "NFiltr", C84->Bci_block.NFiltr);
    WDFunc::SPBData(this, "T_Data_Rec", C84->Bci_block.T_Data_Rec);
    WDFunc::SPBData(this, "Phy_unb_init", C84->Bci_block.Phy_unb_init);
    WDFunc::SPBData(this, "Iunb_init", C84->Bci_block.Iunb_init);
    WDFunc::SPBData(this, "Unom_1", C84->Bci_block.Unom_1);

    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    {
        WDFunc::SPBData(this, "RTerm", C84->Bci_block.RTerm);
        WDFunc::SPBData(this, "W100", C84->Bci_block.W100);

        for (i = 0; i < 4; i++)
        {
         WDFunc::SPBData(this, "IP"+QString::number(i), C84->Com_param.IP[i]);
         WDFunc::SPBData(this, "Mask"+QString::number(i), C84->Com_param.Mask[i]);
         WDFunc::SPBData(this, "GateWay"+QString::number(i), C84->Com_param.GateWay[i]);
         WDFunc::SPBData(this, "Port"+QString::number(i), C84->Com_param.Port[i]);
         WDFunc::SPBData(this, "SNTP"+QString::number(i), C84->Com_param.SNTP[i]);
        }

        WDFunc::SPBData(this, "Baud", C84->Com_param.baud);
        WDFunc::SPBData(this, "Parity", C84->Com_param.parity);
        WDFunc::SPBData(this, "StopBit", C84->Com_param.stopbit);
        WDFunc::SPBData(this, "adrMB", C84->Com_param.adrMB);
        /*if(C84->Com_param.isNTP)
        WDFunc::SetChBData(this, "ISNTP", true);
        else
        WDFunc::SetChBData(this, "ISNTP", false);*/
        WDFunc::ChBData(this, "ISNTP", IsNtp);
        if(IsNtp)
        C84->Com_param.isNTP = 1;
        else
        C84->Com_param.isNTP = 0;

    }


}

void ConfDialog84::SetupUI()
{
    QString phase[3] = {"A:","B:","C:"};
    //QString S;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QWidget *analog1 = new QWidget;
    QWidget *analog2 = new QWidget;
    QWidget *extraconf = new QWidget;
    QWidget *time = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    analog1->setStyleSheet(tmps);
    analog2->setStyleSheet(tmps);
    extraconf->setStyleSheet(tmps);
    time->setStyleSheet(tmps);

    uint32_t unixtimestamp = 1423062000;

    QDateTime myDateTime;

    myDateTime.setTime_t(unixtimestamp);

    QString paramcolor = MAINWINCLR;
    int row = 0;
    QGroupBox *gb = new QGroupBox("Аналоговые параметры");

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное линейное первичное напряжение, кВ:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Unom", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума напряжения (в % от номинального):"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Umin", 0, 10000, 1, paramcolor), row,2,1,3);
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума тока (в % от Imax):"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Imin", 0, 10000, 1, paramcolor), row,2,1,3);
    row++;

    /*for(int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewLBL(this, phase[i]), row,2+i,1,1,Qt::AlignLeft);
    }
    row++;*/
    glyout->addWidget(WDFunc::NewLBL(this, "Паспортные значения ёмкости вводов, пФ:"), row,1,1,1);
    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewLBL(this, phase[i]), row,2+i,1,1,Qt::AlignLeft);
     glyout->addWidget(WDFunc::NewSPB(this, "C_pasp."+QString::number(i), 0, 20000, 0, paramcolor), row,2+i,1,1,Qt::AlignRight);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Паспортные значения tg вводов, %:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewLBL(this, phase[i]), row,2+i,1,1,Qt::AlignLeft);
     glyout->addWidget(WDFunc::NewSPB(this, "Tg_pasp."+QString::number(i), 0, 10000, 1, paramcolor), row,2+i,1,1,Qt::AlignRight);
    }
    row++;
    /*glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения tg δ вводов, %:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "Tg_init."+QString::number(i), 0, 10000, 1, paramcolor), row,2+i,1,1);
    }
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Коррекция  tg δ вводов:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "corTg."+QString::number(i), 0, 10000, 1, paramcolor), row, 2+i, 1, 1);
    }
    row++; */
    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по dC, % от уставки:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "GdС", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по dTg, % от уставки:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "GdTg", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по небалансу токов:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "GdIunb", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное действ. значение тока небаланса:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Iunb_init", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное значение угла тока небаланса:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Phy_unb_init", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение первой тройки:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Unom_1", 0, 10000, 1, paramcolor), row,2,1,3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    /*gb = new QGroupBox("Осциллограммы");
    vlyout2 = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Запуск осциллограммы:"));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.0", "по команде Ц", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.1", "по дискр. входу PD1", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.2", "по резкому изменению", ACONFWCLR));
    vlyout2->addLayout(hlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);*/
    analog1->setLayout(vlyout1);


    gb = new QGroupBox("Уставки");
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    row = 0;

    glyout->addWidget(WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по изменению емкости:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dС_pred", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка аварийной сигнализации по изменению емкости:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dС_alarm", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по изменению tg δ:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dTg_pred", 0, 1000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка аварийной сигнализации по изменению tg δ:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dTg_alarm", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, " Уставка предупредительной сигнализации по изменению небаланса токов:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dIunb_pred", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка аварийной сигнализации по изменению небаланса токов:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dIunb _alarm", 0, 10000, 1, paramcolor), row,2,1,3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Параметры записи");
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Интервал усреднения данных  (в периодах основной частоты):"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "NFiltr", 0, 10000, 0, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Интервал записи данных в ПЗУ (тренд), в секундах:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "T_Data_Rec", 0, 10000, 0, paramcolor), row,2,1,3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    {
        gb = new QGroupBox("Температура");
        vlyout2 = new QVBoxLayout;
        glyout = new QGridLayout;

        row++;
        glyout->addWidget(WDFunc::NewLBL(this, "Сопротивление термометра при 0°С, Ом (только для МНК3):"), row,1,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "RTerm", 0, 10000, 1, paramcolor), row,2,1,3);

        row++;
        glyout->addWidget(WDFunc::NewLBL(this, "Температурный коэффициент термометра (только для МНК3):"), row,1,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "W100", 0, 10000, 3, paramcolor), row,2,1,3);

        vlyout2->addLayout(glyout);
        gb->setLayout(vlyout2);
        vlyout1->addWidget(gb);
    }

    analog2->setLayout(vlyout1);

    gb = new QGroupBox("Конфигурация 104");
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    row = 0;

    glyout->addWidget(WDFunc::NewLBL(this, "IP адрес устройства:"), row,1,1,1);

    for (int i = 0; i < 4; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "IP"+QString::number(i), 0, 10000, 0, paramcolor), row,2+i,1,1, Qt::AlignLeft);
    }

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Маска:"), row,1,1,1);

    for (int i = 0; i < 4; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "Mask"+QString::number(i), 0, 10000, 0, paramcolor), row,2+i,1,1, Qt::AlignLeft);
    }

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Шлюз:"), row,1,1,1);

    for (int i = 0; i < 4; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "GateWay"+QString::number(i), 0, 10000, 0, paramcolor), row,2+i,1,1, Qt::AlignLeft);
    }

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Порты протоколов:"), row,1,1,1);

    for (int i = 0; i < 4; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "Port"+QString::number(i), 0, 10000, 0, paramcolor), row,2+i,1,1, Qt::AlignLeft);
    }

    row++;
    glyout->addWidget(WDFunc::NewChB(this, "ISNTP", "Использование SNTP сервера"), row,1,1,1);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Адрес SNTP сервера:"), row,1,1,1);

    for (int i = 0; i < 4; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "SNTP"+QString::number(i), 0, 10000, 0, paramcolor), row,2+i,1,1, Qt::AlignLeft);
    }

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Скорость 485 интерфейса:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Baud", 0, 100000, 0, paramcolor), row,2,1,4);


    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Чётность:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Parity", 0, 10000, 0, paramcolor), row,2,1,4);


    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Количество стоповых битов:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "StopBit", 0, 10000, 0, paramcolor), row,2,1,4);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Адрес устройства для Modbus:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "adrMB", 0, 10000, 0, paramcolor), row,2,1,4);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    extraconf->setLayout(vlyout1);

    gb = new QGroupBox("");
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    row = 0;

    SysTime = new QLabel;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slot_timeOut()));

    //SysTime->setText(QTime::currentTime().toString("hh:mm:ss"));

    timer->start(1000);

    QString Text = "dd-MM-yyyy HH:mm:ss";

    QDateTime current = QDateTime::currentDateTime();
    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время ПК:"), row,1,1,1, Qt::AlignTop);
    QDateTime dt = QDateTime::currentDateTime();
    //SysTime->setText(dt.toString());
    SysTime->setText(QDateTime::currentDateTimeUtc().toString("yyyy-MM-ddTHH:mm:ss"));
    glyout->addWidget(SysTime, row,2,1,4, Qt::AlignTop);

    row++;
    tmps = "QWidget {background-color: "+QString(MAINWINCLR)+";}";
    QPushButton *Button = new QPushButton("Записать дату и время ПК в модуль");
    Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row,1,1,6, Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Write_PCDate()));

    SysTime2 = new QLabel;
    timerRead = new QTimer(this);
    connect(timerRead, SIGNAL(timeout()), this, SLOT(slot2_timeOut()));
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время в модуле:"), row,1,1,1);
    SysTime2->setText(Text);
    glyout->addWidget(SysTime2, row,2,1,4);

    row++;
    Button = new QPushButton("Читать время из модуля");
    Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row,1,1,1,Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Start_Timer()));
    Button = new QPushButton("Остановить чтение");
    Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row,2,1,5,Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Stop_Timer()));

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Дата и время для записи в модуль"), row,1,1,1);

    //for(int i = 0; i<3; i++)
    //{
    //glyout->addWidget(WDFunc::NewSPB(Date, "Date", 0, 100000, 0, paramcolor), row,2,1,1);
    glyout->addWidget(WDFunc::NewLE(this, "Date", Text, paramcolor), row,2,1,1);

    //}

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "день-месяц-год часы:минуты:секунды"), row,2,1,1);

    /*for(int i = 0; i<3; i++)
    {
       glyout->addWidget(WDFunc::NewSPB(this, "Time"+QString::number(i), 0, 59, 0, paramcolor), row,2+i,1,1);
    }*/


    row++;
    Button = new QPushButton("Записать заданное время в модуль");
    Button->setStyleSheet(tmps);
    glyout->addWidget(Button, row,1,1,6, Qt::AlignTop);
    connect(Button, SIGNAL(clicked()), this, SLOT(Write_Date()));

    //SysTime2->setText(QTime::currentTime().toString("hh:mm:ss"));
    // glyout->addWidget(SysTime2, row,2,1,4, Qt::AlignTop);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    time->setLayout(vlyout1);

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(analog1,"Аналоговые");

    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    {
      ConfTW->addTab(analog2,"Уставки и температура");
      ConfTW->addTab(extraconf,"Связь");
    }
    else
    ConfTW->addTab(analog2,"Уставки");

    ConfTW->addTab(time,"Время");
    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfDialog84::CheckConf()
{
}




void ConfDialog84::SetDefConf()
{
    C84->SetDefConf();
}

void ConfDialog84::slot_timeOut()
{
   // WDFunc::LBLText(this, "adrMB", currentTime().toString("hh:mm:ss"));
    SysTime->setText(QDateTime::currentDateTimeUtc().toString("dd-MM-yyyy HH:mm:ss"));
    //SysTime->update();
}

void ConfDialog84::slot2_timeOut()
{
    uint unixtimestamp = 0;
    QDateTime myDateTime;

    #if PROGSIZE != PROGSIZE_EMUL
    if (Commands::GetTimeMNK(unixtimestamp) == Error::ER_NOERROR)
    {
      myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::UTC);
      //myDateTime.setTime_t(unixtimestamp);
      SysTime2->setText(myDateTime.toString("dd-MM-yyyy HH:mm:ss"));

      //EMessageBox::information(this, "INFO", "Прочитано успешно");
    }
    //else
    //EMessageBox::information(this, "INFO", "Ошибка");
    #endif

}

void ConfDialog84::Start_Timer()
{
   timerRead->start(1000);
}

void ConfDialog84::Stop_Timer()
{
   timerRead->stop();
}

void ConfDialog84::Write_PCDate()
{
    QDateTime myDateTime;
    uint time ;
    myDateTime = QDateTime::currentDateTimeUtc();
    time = myDateTime.toTime_t();

    #if PROGSIZE != PROGSIZE_EMUL
    if (Commands::WriteTimeMNK(&time, sizeof(uint)) == Error::ER_NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");
    #endif

}

void ConfDialog84::Write_Date()
{
    QDateTime myDateTime;
    uint *time = new uint;
    QString qStr;
    WDFunc::LE_read_data(this, "Date", qStr);
    myDateTime = QDateTime::fromString(qStr,"dd-MM-yyyy HH:mm:ss");
    myDateTime.setOffsetFromUtc(0);
    *time = myDateTime.toTime_t();

    #if PROGSIZE != PROGSIZE_EMUL
    if (Commands::WriteTimeMNK(time, sizeof(uint)) == Error::ER_NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");
    #endif

}

/*void AbstractConfDialog::Fill_tg_init(float *tginit)
{
    int i;

    for (i = 0; i < 3; i++)
    {
      WDFunc::SetSPBData(this, "Tg_init."+QString::number(i), *(tginit+i));
    }
}*/
