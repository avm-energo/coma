/* A1Dialog - dialog providing a main function of the device
 *
*/
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QGroupBox>
#include <QStringListModel>
#include <QFileDialog>
#include <QPushButton>
#include <QTime>
#include "../canal.h"
#include "a1dialog.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

A1Dialog::A1Dialog(QWidget *parent) : QDialog(parent)
{
    ChA1 = new CheckA1;
    CA1 = new ConfigA1(S2Config);

    SetupUI();
    MeasurementTimer = new QTimer;
    connect(MeasurementTimer,SIGNAL(timeout()),this,SLOT(MeasTimerTimeout()));
}

void A1Dialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Измерения в первичном масштабе");
    glyout->addWidget(WDFunc::NewLBL(this, "U1"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu1", ValuesFormat, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "U2"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu2", ValuesFormat, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "δU, %"), 0, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunepercent", ValuesFormat, ""), 0, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Phy"), 1, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednphy", ValuesFormat, ""), 1, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Freq"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednfreq", ValuesFormat, ""), 1, 1, 1, 1);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(5, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    QPushButton *pb = new QPushButton("Подтвердить");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Accept()));
    QHBoxLayout *hlyout = new QHBoxLayout;
    pb->setObjectName("acceptpb");
    pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Decline()));
    pb->setObjectName("cancelpb");
    pb->setEnabled(false);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    lyout->addStretch(1);
    pb = new QPushButton("Начать поверку делителя");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(StartWork()));
    lyout->addWidget(pb);
    lyout->addStretch(10);
    setLayout(lyout);
}

int A1Dialog::GetConf()
{
    cn->Send(CN_GF, BT_NONE,NULL,0,1,&S2Config); // заполнение CA1->Bci_block
    if (cn->result == NOERROR)
    {
        cn->Send(CN_GBac, BT_MEZONIN, &Bac_block, sizeof(Bac));
        if (cn->result == NOERROR)
        {
            Bac_block.U1kDN[0] = 0;
            Bac_block.U2kDN[0] = 0;
            Bac_block.PhyDN[0] = 0;
            return NOERROR;
        }
    }
    return GENERALERROR;
}

void A1Dialog::FillBdOut()
{
    WDFunc::SetLBLText(this, "tunednu1", QString::number(ChA1->Bda_out.Uef_filt[0], 'f', 5));
    WDFunc::SetLBLText(this, "tunednu2", QString::number(ChA1->Bda_out.Uef_filt[1], 'f', 5));
    WDFunc::SetLBLText(this, "tunednphy", QString::number(ChA1->Bda_out.Phy, 'f', 5));
    WDFunc::SetLBLText(this, "tunednfreq", QString::number(ChA1->Bda_out.Frequency, 'f', 5));
    WDFunc::SetLBLText(this, "tunepercent", QString::number(ChA1->Bda_out.dUrms, 'f', 5));
}

void A1Dialog::StartWork()
{
    PovType = GOST_NONE;
    while (PovType == GOST_NONE)
    {
        QTime tme;
        tme.start();
    }
/*    report = new LimeReport::ReportEngine();
    report->loadFromFile(pc.SystemHomeDir+"a1.lrxml");
    report->dataManager()->setReportVariable("Organization", pc.OrganizationString);
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QString Filename = dlg->getSaveFileName(this, "", pc.HomeDir, "*.pdf");
    report->printToPDF(Filename); */

/*    if (GetConf() != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Ошибка чтения конфигурации или настроечных параметров из модуля");
        return;
    }
    if (MessageBox2::question(this, "Подтверждение", "Подключите вывод нижнего плеча \"своего\" делителя напряжения ко входу U1 прибора\n"
                              "Вывод нижнего плеча поверяемого делителя или выход низшего напряжения поверяемого ТН - ко входу U2") == false)
        return;
    float VoltageInkV = static_cast<float>(CA1->Bci_block.K_DN) * 20 / 1732;
    if (MessageBox2::question(this, "Подтверждение", "Подайте на делители напряжение " + QString::number(VoltageInkV, 'f', 1) + " кВ") == false)
        return;
    Counter = 0;
    MeasurementTimer->start();
    WDFunc::SetEnabled(this, "cancelpb", true);
    WDFunc::SetEnabled(this, "acceptpb", true); */
}

void A1Dialog::MeasTimerTimeout()
{
    cn->Send(CN_GBd, 4, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1));
    if (cn->result == NOERROR)
        FillBdOut();
}

void A1Dialog::Accept()
{
    MeasurementTimer->stop();
    int Pindex = (Counter > 4) ? (8 - Counter) : Counter;
    const int Percents[] = {20, 50, 80, 100, 120, 100, 80, 50, 20};
    Results[Counter].K = Percents[Counter];
    Results[Counter].dUp = Bac_block.dU_cor[Pindex];
    Results[Counter].dPp = Bac_block.dPhy_cor[Pindex];
    Results[Counter].dUd = ChA1->Bda_out.dUrms;
    Results[Counter].dPd = ChA1->Bda_out.Phy;
    ++Counter;
    if (Counter >= 9)
    {
        // запись файла протокола
        // вывод протокола на экран
        // формирование отчёта
        Decline();
        return;
    }
    float VoltageInkV = static_cast<float>(CA1->Bci_block.K_DN) * Percents[Counter] / 1732;
    if (MessageBox2::question(this, "Подтверждение", "Подайте на делители напряжение " + QString::number(VoltageInkV, 'f', 1) + " кВ") == false)
        Decline();
    MeasurementTimer->start();
}

void A1Dialog::Decline()
{
    WDFunc::SetEnabled(this, "cancelpb", false);
    WDFunc::SetEnabled(this, "acceptpb", false);
    MeasurementTimer->stop();
}

