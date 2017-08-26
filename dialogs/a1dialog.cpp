/* A1Dialog - dialog providing a main function of the device
 *
*/
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QGroupBox>
#include <QRadioButton>
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
    ReportModel = new QStandardItemModel;
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
    glyout->addLayout(hlyout, 2, 0, 1, 6);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(5, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    pb = new QPushButton("Сформировать протокол из файла ПКДН");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(ParsePKDNFile()));
    lyout->addWidget(pb);
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

void A1Dialog::GenerateReport()
{
    ConditionDataDialog(); // задаём условия поверки
    DNDialog(); // вводим данные по делителю
    // данные в таблицу уже получены или из файла, или в процессе работы
    QString GOST = (PovType == GOST_1983) ? "1983" : "23625";
    report = new LimeReport::ReportEngine(this);
/*    int RowCount = 3;
    int ColumnCount = 14;
    QStandardItemModel *mdl = new QStandardItemModel(RowCount, ColumnCount);
    for (int i=0; i<RowCount; ++i)
    {
        for (int j=0; j<ColumnCount; ++j)
        {
            QStandardItem *item = new QStandardItem(QString::number(ColumnCount*i+j));
            mdl->setItem(i, j, item);
        }
    } */
    report->loadFromFile(pc.SystemHomeDir+"a1_"+GOST+".lrxml");
    report->dataManager()->addModel("maindata", ReportModel, false);
    report->dataManager()->setReportVariable("Organization", pc.OrganizationString);
/*    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QString Filename = dlg->getSaveFileName(this, "", pc.HomeDir, "*.pdf");
    dlg->close();
    report->printToPDF(Filename); */
    report->previewReport();
//    report->designReport();
}

void A1Dialog::ConditionDataDialog()
{
    int row = 0;
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Условия поверки"), Qt::AlignCenter);
    if (CA1->Bci_block.DTCanal == 0)
    {
        glyout->addWidget(WDFunc::NewLBL(this, "Температура окружающей среды, °С"), row, 0, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLEF(this, "Temp", ""), row++, 1, 1, 1, Qt::AlignLeft);
    }
    if (CA1->Bci_block.DHCanal == 0)
    {
        glyout->addWidget(WDFunc::NewLBL(this, "Влажность воздуха, %"), row, 0, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLEF(this, "Humidity", ""), row++, 1, 1, 1, Qt::AlignLeft);
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Атмосферное давление, кПа"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "Pressure", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Напряжение питания сети, В"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "Voltage", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Частота питания сети, Гц"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "Frequency", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Коэффициент искажения синусоидальности, %"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "KNI", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->setColumnStretch(1, 1);
    lyout->addLayout(glyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetConditionData()));
    connect(this,SIGNAL(CloseDialog()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}

void A1Dialog::DNDialog()
{
    int row = 0;
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Условия поверки"), Qt::AlignCenter);
    if (CA1->Bci_block.DTCanal == 0)
    {
        glyout->addWidget(WDFunc::NewLBL(this, "Температура окружающей среды, °С"), row, 0, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLEF(this, "Temp", ""), row++, 1, 1, 1, Qt::AlignLeft);
    }
    if (CA1->Bci_block.DHCanal == 0)
    {
        glyout->addWidget(WDFunc::NewLBL(this, "Влажность воздуха, %"), row, 0, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLEF(this, "Humidity", ""), row++, 1, 1, 1, Qt::AlignLeft);
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Атмосферное давление, кПа"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "Pressure", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Напряжение питания сети, В"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "Voltage", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Частота питания сети, Гц"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "Frequency", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Коэффициент искажения синусоидальности, %"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "KNI", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->setColumnStretch(1, 1);
    lyout->addLayout(glyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetDNData()));
    connect(this,SIGNAL(CloseDialog()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}

void A1Dialog::UpdateItemInModel(int row, int column, QVariant value)
{
    QStandardItem *item = ReportModel->item(row, column);
    item->setText(value.toString());
    ReportModel->setItem(row, column, item);
}

void A1Dialog::StartWork()
{
    float VoltageInkV;
    Cancelled = false;
/*    if (GetConf() != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Ошибка чтения конфигурации или настроечных параметров из модуля");
        return;
    } */
    PovType = TempPovType = GOST_NONE;
    QDialog *dlg = new QDialog(this);
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Выберите тип поверяемого оборудования"));
    QRadioButton *rb = new QRadioButton("Трансформаторы напряжения измерительные лабораторные по ГОСТ 23625-2001");
    rb->setObjectName("rb1");
    connect(rb,SIGNAL(toggled(bool)),this,SLOT(RBToggled()));
    lyout->addWidget(rb);
    rb = new QRadioButton("Трансформаторы напряжения по ГОСТ 1983-2001");
    rb->setObjectName("rb2");
    connect(rb,SIGNAL(toggled(bool)),this,SLOT(RBToggled()));
    lyout->addWidget(rb);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Proceed()));
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Cancel()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->show();
    while ((PovType == GOST_NONE) && !Cancelled)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < SLEEPINT)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    dlg->close();
    RowCount = (PovType == GOST_1983) ? GOST1983ROWCOUNT : GOST23625ROWCOUNT;
    ColumnCount = (PovType == GOST_1983) ? GOST1983COLCOUNT : GOST23625COLCOUNT;
    ReportModel->setColumnCount(ColumnCount);
    ReportModel->setRowCount(RowCount);
    for (int i=0; i<RowCount; ++i)
    {
        for (int j=0; j<ColumnCount; ++j)
        {
            QStandardItem *item = new QStandardItem("");
            ReportModel->setItem(i, j, item);
        }
    }
    if (!Cancelled)
    {
        if (MessageBox2::question(this, "Подтверждение", "Подключите вывод нижнего плеча \"своего\" делителя напряжения ко входу U1 прибора\n"
                                  "Вывод нижнего плеча поверяемого делителя или выход низшего напряжения поверяемого ТН - ко входу U2\n"
                                  "На нагрузочном устройстве поверяемого ТН установите значение мощности, равное 0,25·Sном") == true)
        {
            CurrentS = 0.25;
            if (PovType == GOST_1983)
                VoltageInkV = static_cast<float>(CA1->Bci_block.K_DN) * 80 / 1732;
            else
                VoltageInkV = static_cast<float>(CA1->Bci_block.K_DN) * 20 / 1732;
            if (MessageBox2::question(this, "Подтверждение", "Подайте на делители напряжение " + QString::number(VoltageInkV, 'f', 1) + " кВ") == true)
            {
                Counter = 0;
                MeasurementTimer->start();
                WDFunc::SetEnabled(this, "cancelpb", true);
                WDFunc::SetEnabled(this, "acceptpb", true);
                return;
            }
        }
    }
    MessageBox2::information(this, "Информация", "Операция прервана");
    return;
}

void A1Dialog::ParsePKDNFile()
{
/*    QByteArray ba;
    int res = pc.LoadFile(this, "PKDN verification files (*.vrf)", ba);
    if (res != NOERROR)
        return; */

    GenerateReport();
}

void A1Dialog::MeasTimerTimeout()
{
//    cn->Send(CN_GBd, A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1));
    if (cn->result == NOERROR)
        FillBdOut();
}

void A1Dialog::Accept()
{
    float VoltageInkV;
    int endcounter = (PovType == GOST_1983) ? 3 : 9;
    MeasurementTimer->stop();
    int Pindex = (Counter > 4) ? (8 - Counter) : Counter;
    const int Percents23625[] = {20, 50, 80, 100, 120};
    const int Percents1983[] = {80, 100, 120};
    const int *Percents = (PovType == GOST_1983) ? Percents1983 : Percents23625;

    // заполняем модель по полученным измерениям:
    // 0 - U/Un (%), 1 - S, 2 -
    int row = Pindex;
    if (CurrentS == 1)
    {
        if (PovType == GOST_1983)
            row += GOST1983ROWCOUNT/2;
        else
            row += GOST23625ROWCOUNT/2;
    }
    if (Counter > 4) // на нисходящем отрезке по ГОСТ 23625
    {
        UpdateItemInModel(row, 4, ChA1->Bda_out.dUrms);
        UpdateItemInModel(row, 5, ChA1->Bda_out.Phy);
        float dUrmsU = ReportModel->item(row, 2)->text().toFloat();
        float PhyU = ReportModel->item(row, 3)->text().toFloat();
        float UrmsM = (dUrmsU + ChA1->Bda_out.dUrms) / 2;
        float PhyM = (PhyU + ChA1->Bda_out.Phy) / 2;
        UpdateItemInModel(row, 6, UrmsM);
        UpdateItemInModel(row, 7, PhyM);
        UpdateItemInModel(row, 8, (ChA1->Bda_out.dUrms - dUrmsU));
        UpdateItemInModel(row, 9, (ChA1->Bda_out.Phy - PhyU));
    }
    else
    {
        int column = 0;
        UpdateItemInModel(row, column++, Percents[Pindex]);
        UpdateItemInModel(row, column++, CurrentS);
        UpdateItemInModel(row, column++, ChA1->Bda_out.dUrms);
        UpdateItemInModel(row, column++, ChA1->Bda_out.Phy);
        if (PovType == GOST_23625)
            column += 6;
        UpdateItemInModel(row, column++, Bac_block.dU_cor[Pindex]);
        UpdateItemInModel(row, column++, Bac_block.dPhy_cor[Pindex]);
        UpdateItemInModel(row, column++, ChA1->Bda_out.dUrms);
        UpdateItemInModel(row, column++, ChA1->Bda_out.Phy);
    }
    ++Counter;
    if (Counter >= endcounter)
    {
        if (CurrentS == 0.25)
        {
            Counter = 0;
            CurrentS = 1;
            if (MessageBox2::question(this, "Подтверждение", "На нагрузочном устройстве поверяемого ТН установите значение мощности, равное 1,0·Sном") == false)
            {
                Cancelled = true;
                Decline();
            }
        }
        else
        {
            // запись файла протокола
            GenerateReport();
            // вывод протокола на экран
            // формирование отчёта
            Decline();
            return;
        }
    }
    Pindex = (Counter > 4) ? (8 - Counter) : Counter;
    VoltageInkV = static_cast<float>(CA1->Bci_block.K_DN) * Percents[Pindex] / 1732;
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

void A1Dialog::Proceed()
{
    PovType = TempPovType;
}

void A1Dialog::Cancel()
{
    Cancelled = true;
}

void A1Dialog::SetDNData()
{
    emit CloseDialog();
}

void A1Dialog::SetConditionData()
{
    cn->Send(CN_GBd, A1_BDA_OUT_AN_BN, &ChA1->Bda_out_an, sizeof(CheckA1::A1_Bd4));
    if (CA1->Bci_block.DTCanal == 0)
        WDFunc::LEData(this, "Temp", ReportHeader.Temp);
    else
        ReportHeader.Temp = ChA1->Bda_out_an.Tamb;
    if (CA1->Bci_block.DHCanal == 0)
        WDFunc::LEData(this, "Humidity", ReportHeader.Humidity);
    else
        ReportHeader.Humidity = ChA1->Bda_out_an.Hamb;
    WDFunc::LEData(this, "Pressure", ReportHeader.Pressure);
    WDFunc::LEData(this, "Voltage", ReportHeader.Voltage);
    WDFunc::LEData(this, "Frequency", ReportHeader.Freq);
    WDFunc::LEData(this, "KNI", ReportHeader.KNI);
    emit CloseDialog();
}

void A1Dialog::RBToggled()
{
    QString tmps = sender()->objectName();
    if (tmps == "rb1")
        TempPovType = GOST_23625;
    else
        TempPovType = GOST_1983;
}
