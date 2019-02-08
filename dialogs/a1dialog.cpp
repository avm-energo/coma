/* A1Dialog - dialog providing a main function of the device
 *
*/
#include <QtMath>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QGroupBox>
#include <QRadioButton>
#include <QStringListModel>
#include <QPushButton>
#include <QTableView>
#include <QTime>
#include <QTimer>
#include "a1dialog.h"
#include "../widgets/emessagebox.h"
#include "../widgets/waitwidget.h"
#include "../widgets/wd_func.h"
#include "../gen/maindef.h"
#include "../gen/stdfunc.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/timefunc.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

A1Dialog::A1Dialog(const QString &filename, QWidget *parent) : EAbstractTuneDialogA1DN(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ChA1 = new CheckA1;
    CA1 = new ConfigA1(S2Config);
    RepModel = new ReportModel;
//    ViewModel = new QStandardItemModel;
    if (filename.isEmpty())
        SetupUI();
    else
    {
        ParsePKDNFile(filename);
        this->close();
        return;
    }
    TuneVariant = 0;
#if PROGSIZE != PROGSIZE_EMUL
/*    MeasurementTimer = new QTimer;
    MeasurementTimer->setInterval(500);
    connect(MeasurementTimer,SIGNAL(timeout()),this,SLOT(MeasTimerTimeout())); */
//    ReportHeader.DNDevices = "УКДН сер. номер " + QString::number(pc.ModuleBsi.SerialNum) + ", кл. точн. 0,05";
    // считать варианты использования и соответствующие им коэффициенты из модуля
    if (GetConf() != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка чтения конфигурации или настроечных параметров из модуля");
        return;
    }
#endif
    WDFunc::SetLBLText(this, "tunevarcoef1", QString::number(Bac_block2.Bac_block2[0].K_DN, 'f', 0));
    WDFunc::SetLBLText(this, "tunevarcoef2", QString::number(Bac_block2.Bac_block2[1].K_DN, 'f', 0));
    WDFunc::SetLBLText(this, "tunevarcoef3", QString::number(Bac_block2.Bac_block2[2].K_DN, 'f', 0));
}

A1Dialog::~A1Dialog()
{
}

void A1Dialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Конфигурация");
    glyout->addWidget(WDFunc::NewLBL(this, "Варианты использования:"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "1", "tunevar1", ValuesFormat, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "2", "tunevar2", ValuesFormat, ""), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "3", "tunevar3", ValuesFormat, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunevarcoef1", ValuesFormat, ""), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunevarcoef2", ValuesFormat, ""), 1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunevarcoef3", ValuesFormat, ""), 1, 3, 1, 1);
    glyout->setColumnStretch(0, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    glyout = new QGridLayout;
    gb = new QGroupBox("Измерения в первичном масштабе");
    glyout->addWidget(WDFunc::NewLBL(this, "U1, кВ"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu1", ValuesFormat, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "U2, кВ"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu2", ValuesFormat, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "δU, %"), 0, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunepercent", ValuesFormat, ""), 0, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Phy, мин"), 1, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednphy", ValuesFormat, ""), 1, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Freq, Гц"), 1, 0, 1, 1, Qt::AlignRight);
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

    glyout = new QGridLayout;
    gb = new QGroupBox("СКО");
    glyout->addWidget(WDFunc::NewLBL(this, "dUrms(m), В"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunedurmsm", ValuesFormat, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "φ(m), мин"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunephym", ValuesFormat, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "σUrms, %"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunesurms", ValuesFormat, ""), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "σφ, мин"), 1, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunesphy", ValuesFormat, ""), 1, 3, 1, 1);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);

    pb = new QPushButton("Начать поверку делителя");
    pb->setObjectName("StartWorkPb");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(StartWork()));
#endif
//    connect(pb,SIGNAL(clicked(bool)),this,SLOT(TempRandomizeModel()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    lyout->addWidget(pb);
    lyout->addStretch(10);
    setLayout(lyout);
}

#if PROGSIZE != PROGSIZE_EMUL
void A1Dialog::StartWork()
{
    TuneVariant = 0;
    Autonomous = false;
//    float VoltageInkV, VoltageInV;
//    Cancelled = false;
    StdFunc::ClearCancel();
    TemplateCheck();
    if (GetConf() != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка чтения конфигурации или настроечных параметров из модуля");
        return;
    }
    WDFunc::SetEnabled(this, "StartWorkPb", false);
    PovType = TempPovType = GOST_NONE;
    InputTuneParameters(DNT_FOREIGN);
    if (StdFunc::IsCancelled())
        return;
    if (Commands::SetUsingVariant(TuneVariant+1) != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка установки варианта использования");
        return;
    }
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
    while ((PovType == GOST_NONE) && !StdFunc::IsCancelled())
        TimeFunc::Wait();
    dlg->close();
    int rowcount = (PovType == GOST_1983) ? GOST1983ROWCOUNT : GOST23625ROWCOUNT;
    int columncount = (PovType == GOST_1983) ? GOST1983COLCOUNT : GOST23625COLCOUNT;
    RepModel->SetModel(rowcount, columncount);
    if (!StdFunc::IsCancelled())
    {
        if (EMessageBox::question(this, "Подтверждение", "Подключите вывод нижнего плеча \"своего\" делителя напряжения ко входу U1 прибора\n"
                                  "Вывод нижнего плеча поверяемого делителя или выход низшего напряжения поверяемого ТН - ко входу U2\n"
                                  "На нагрузочном устройстве поверяемого ТН установите значение мощности, равное 0,25·Sном") == true)
        {
            CurrentS = 0.25;
            Index = 0;
            Counter = 0;
            WDFunc::SetEnabled(this, "cancelpb", true);
            WDFunc::SetEnabled(this, "acceptpb", true);
            int percent = (PovType == GOST_1983) ? 80 : 20;
            if (ShowVoltageDialog(percent) == Error::ER_NOERROR)
            {
                MeasurementTimer->start();
                return;
            }
/*            if (PovType == GOST_1983)
            {
                VoltageInkV = static_cast<float>(Bac_block.Bac_block[TuneVariant].K_DN) * 80 / 1732;
                VoltageInV = static_cast<float>(1000 * 80) / 1732;
            }
            else
            {
                VoltageInkV = static_cast<float>(Bac_block.Bac_block[TuneVariant].K_DN) * 20 / 1732;
                VoltageInV = static_cast<float>(1000 * 20) / 1732;
            }
            if (EMessageBox::question(this, "Подтверждение", "Подайте на делители напряжение " + \
                                      QString::number(VoltageInkV, 'f', 1) + " кВ ("+QString::number(VoltageInV, 'f', 1)+" В)") == true)
            {
                MeasurementTimer->start();
                return;
            } */
        }
    }
    WDFunc::SetEnabled(this, "StartWorkPb", true);
    EMessageBox::information(this, "Информация", "Операция прервана");
}
#endif

#if PROGSIZE != PROGSIZE_EMUL
int A1Dialog::GetConf()
{
    if (Commands::GetFile(1, &S2Config) == Error::ER_NOERROR)
    {
        if (Commands::GetBac(2, &Bac_block2, sizeof(Bac2)) == Error::ER_NOERROR)
        {
            Bac_block2.Bac_block2[TuneVariant].U1kDN[0] = 0;
            Bac_block2.Bac_block2[TuneVariant].U2kDN[0] = 0;
            Bac_block2.Bac_block2[TuneVariant].PhyDN[0] = 0;
            return Error::ER_NOERROR;
        }
    }
    return Error::ER_GENERALERROR;
}
#endif
/*void A1Dialog::FillBdOut()
{
    WDFunc::SetLBLText(this, "tunednu1", QString::number(ChA1->Bda_out.Uef_filt[0], 'f', 5));
    WDFunc::SetLBLText(this, "tunednu2", QString::number(ChA1->Bda_out.Uef_filt[1], 'f', 5));
    WDFunc::SetLBLText(this, "tunednphy", QString::number((-ChA1->Bda_out.Phy), 'f', 5));
    WDFunc::SetLBLText(this, "tunednfreq", QString::number(ChA1->Bda_out.Frequency, 'f', 5));
    WDFunc::SetLBLText(this, "tunepercent", QString::number((-ChA1->Bda_out.dUrms), 'f', 5));
} */

/*void A1Dialog::FillMedian()
{
    WDFunc::SetLBLText(this, "tunedurmsm", QString::number(Dd_Block.dUrms, 'f', 5));
    WDFunc::SetLBLText(this, "tunephym", QString::number(Dd_Block.Phy, 'f', 5));
    WDFunc::SetLBLText(this, "tunesurms", QString::number(Dd_Block.sU, 'f', 5));
    WDFunc::SetLBLText(this, "tunesphy", QString::number(Dd_Block.sPhy, 'f', 5));
} */

void A1Dialog::GenerateReport()
{
    // данные в таблицу уже получены или из файла, или в процессе работы
    // отобразим таблицу
    FillHeaders();
    ShowTable();
    QString GOST = (PovType == GOST_1983) ? "1983" : "23625";
    Report *report = new Report(GOST, this);
    report->AddModel("maindata", RepModel);
    // запрос блока Bda_h, чтобы выдать KNI в протокол
#if PROGSIZE != PROGSIZE_EMUL
    if (!Autonomous)
    {
        if (Commands::GetBd(A1_BDA_H_BN, &ChA1->Bda_h, sizeof(CheckA1::A1_Bd2)) == Error::ER_NOERROR)
            report->SetVar("KNI", ChA1->Bda_h.HarmBuf[0][0], 5);
    }
    else
#endif
        report->SetVar("KNI", ChA1->Bda_h.HarmBuf[0][0], 5);
    report->SetVar("Organization", StdFunc::OrganizationString());
    QString day = QDateTime::currentDateTime().toString("dd");
    QString month = QDateTime::currentDateTime().toString("MM");
    QString yr = QDateTime::currentDateTime().toString("yy");
    report->SetVar("Day", day);
    report->SetVar("Month", month);
    report->SetVar("Yr", yr);
    report->SetVar("DNNamePhase", ReportHeader.DNNamePhase);
    report->SetVar("DNType", ReportHeader.DNType);
    report->SetVar("DNSerNum", ReportHeader.DNSerNum);
    report->SetVar("DNTol", ReportHeader.DNTol);
    report->SetVar("DNU1", ReportHeader.DNU1);
    report->SetVar("DNU2", ReportHeader.DNU2);
    report->SetVar("DNP", ReportHeader.DNP);
    report->SetVar("DNF", ReportHeader.DNF);
    report->SetVar("DNOrganization", ReportHeader.DNOrganization);
    report->SetVar("DNPlace", ReportHeader.DNPlace);
    report->SetVar("DNDevices", ReportHeader.DNDevices);
    report->SetVar("Temp", ReportHeader.Temp);
    report->SetVar("Humidity", ReportHeader.Humidity);
    report->SetVar("Pressure", ReportHeader.Pressure);
    report->SetVar("Voltage", ReportHeader.Voltage);
    report->SetVar("Freq", ReportHeader.Freq);
    report->SetVar("OuterInsp", ReportHeader.OuterInsp);
    report->SetVar("WindingsInsp", ReportHeader.WindingsInsp);
    report->SetVar("PovDateTime", ReportHeader.PovDateTime);
    QString filename = Files::ChooseFileForSave(this, "*.pdf", "pdf");
    if (!filename.isEmpty())
    {
        report->Generate(filename);
        EMessageBox::information(this, "Успешно!", "Записано успешно!");
    }
    else
        EMessageBox::information(this, "Отменено", "Действие отменено");
    delete report;
}

/*bool A1Dialog::ConditionDataDialog()
{
//    Cancelled = false;
    StdFunc::ClearCancel();
    int row = 0;
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Условия поверки"), Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Дата поверки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "PovDateTime", ReportHeader.PovDateTime), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Температура окружающей среды, °С"), row, 0, 1, 1, Qt::AlignRight);
    QString tmps;
    if (Autonomous)
    {
        if (StdFunc::FloatInRange(ReportHeader.Temp.toFloat(), FLT_MAX, TH1))
            tmps = "";
        else
            tmps = ReportHeader.Temp;
    }
    else
    {
        if (StdFunc::FloatInRange(ChA1->Bda_out_an.Tamb, FLT_MAX, TH1))
            tmps = "";
        else
            tmps = ReportHeader.Temp;
    }
    glyout->addWidget(WDFunc::NewLE(this, "Temp", tmps), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Влажность воздуха, %"), row, 0, 1, 1, Qt::AlignRight);
    if (Autonomous)
    {
        if (StdFunc::FloatInRange(ReportHeader.Humidity.toFloat(), FLT_MAX, TH1))
            tmps = "";
        else
            tmps = ReportHeader.Humidity;
    }
    else
    {
        if (StdFunc::FloatInRange(ChA1->Bda_out_an.Hamb, FLT_MAX, TH1))
            tmps = "";
        else
            tmps = ReportHeader.Humidity;
    }
    glyout->addWidget(WDFunc::NewLE(this, "Humidity", tmps), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Атмосферное давление, кПа"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "Pressure", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Напряжение питания сети, В"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "Voltage", ""), row++, 1, 1, 1, Qt::AlignLeft); */
/*    glyout->addWidget(WDFunc::NewLBL(this, "Частота питания сети, Гц"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "Frequency", ReportHeader.Freq), row++, 1, 1, 1, Qt::AlignLeft); */
/*    glyout->setColumnStretch(1, 1);
    lyout->addLayout(glyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetConditionData()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Cancel()));
    lyout->addWidget(pb);
    connect(this,SIGNAL(Finished()),dlg,SLOT(close()));
    dlg->setLayout(lyout);
    dlg->exec();
    return StdFunc::IsCancelled();
}

bool A1Dialog::DNDialog(PovDevStruct &PovDev)
{
//    Cancelled = false;
    StdFunc::ClearCancel();
    int row = 0;
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Данные ТН(ДН)"), Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Организация, проводившая поверку"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "UKDNOrganization", StdFunc::OrganizationString()), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Тип ТН(ДН)"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNType", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Обозначение по схеме, фаза"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNNamePhase", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Заводской номер"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNSerialNum", ReportHeader.DNSerNum), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Класс точности, %"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNTolerance", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное первичное напряжение, кВ"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNU1", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение, В"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNU2", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальная мощность нагрузки, ВА"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNP", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальная частота, Гц"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNFreq", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Предприятие-изготовитель"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNOrganization", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Место установки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNPlace", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Наименование средства поверки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "PovDev", PovDev.DevName), row++, 1, 1, 1, Qt::AlignLeft);
    WDFunc::SetEnabled(this, "PovDev", false);
    glyout->addWidget(WDFunc::NewLBL(this, "Заводской номер средства поверки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "PovDevSN", PovDev.DevSN), row++, 1, 1, 1, Qt::AlignLeft);
    WDFunc::SetEnabled(this, "PovDevSN", false);
    glyout->addWidget(WDFunc::NewLBL(this, "Класс точности средства поверки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "PovDevPrecision", PovDev.DevPrecision), row++, 1, 1, 1, Qt::AlignLeft);
    WDFunc::SetEnabled(this, "PovDevPrecision", false);
    glyout->addWidget(WDFunc::NewLBL(this, "Результаты внешнего осмотра"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNInspection", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Результаты проверки правильности обозначения\nвыводов и групп соединений обмоток"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNWindingInspection", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->setColumnStretch(1, 1);
    lyout->addLayout(glyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetDNData()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Cancel()));
    lyout->addWidget(pb);
    connect(this,SIGNAL(Finished()),dlg,SLOT(close()));
    dlg->setLayout(lyout);
    dlg->exec();
    return StdFunc::IsCancelled();
}

void A1Dialog::ShowTable()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTableView *tw = new QTableView;
    tw->setModel(RepModel);
    lyout->addWidget(tw);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}
*/
void A1Dialog::ParsePKDNFile(const QString &filename)
{
    int rowcount, columncount;
    Autonomous = true;
    QByteArray ba;
    PovDevStruct PovDev;
    int res = Files::LoadFromFile(filename, ba);
    if (res != Files::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка загрузки файла");
        return;
    }
    TemplateCheck();
    // заполняем ReportHeader
    PovDev.DevName = this->PovDev.DevName;
    PovDev.DevPrecision = this->PovDev.DevPrecision;
    if (ba.size() >= sizeof(ResultsStruct))
    {
        ResultsStruct Results;
        memcpy(&Results, &ba.data()[0], sizeof(ResultsStruct));
        PovDev.DevSN = QString::number(Results.DNFNum); // переписываем серийный номер установки для редактора полей протокола
        MainDataStruct MDS;
        int MDSCount;
        if (Results.GOST == 0) // GOST 1983
        {
            MDSCount = 6;
            rowcount = GOST1983ROWCOUNT;
            columncount = GOST1983COLCOUNT;
            PovType = GOST_1983;
        }
        else if (Results.GOST == 1) // GOST 23625
        {
            MDSCount = 18;
            rowcount = GOST23625ROWCOUNT;
            columncount = GOST23625COLCOUNT;
            PovType = GOST_23625;
        }
        else
        {
            EMessageBox::error(this, "Ошибка", "Ошибочный тип трансформатора в файле");
            return;
        }
        // разберём время
        quint32 tmpi = Results.Time >> 32;
        QDateTime tn = QDateTime::fromTime_t(tmpi, Qt::UTC);
        ReportHeader.PovDateTime = tn.toString("dd-MM-yyyy hh:mm:ss");
        ChA1->Bda_h.HarmBuf[0][0] = Results.THD;
        TuneVariant = (Results.VarNum >= TUNEVARIANTSNUM) ? 0 : Results.VarNum;
        ReportHeader.Freq = QString::number(Results.Frequency, 'f', 5);
        ReportHeader.Humidity = QString::number(Results.Humidity, 'f', 5);
        ReportHeader.Temp = QString::number(Results.Temp, 'f', 5);
        ReportHeader.DNSerNum = QString::number(Results.SerNum);
        RepModel->SetModel(rowcount, columncount);
        int memptr, MDSs;
        memptr = sizeof(ResultsStruct);
        MDSs = sizeof(MainDataStruct);
        Index = 0;
        int endcounter = (PovType == GOST_1983) ? 3 : 9;
        for (int i=0; i<MDSCount; ++i)
        {
            int Pindex = (Index > 4) ? (8 - Index) : Index;
            if (memptr > (ba.size() - MDSs))
            {
                EMessageBox::error(this, "Ошибка", "Неожиданный конец файла");
                return;
            }
            memcpy(&MDS, &ba.data()[memptr], MDSs);
            // заполним модель
            CurrentS = MDS.S;
            Dd_Block[Index].dUrms = MDS.dUp;
            Dd_Block[Index].Phy = MDS.dPp;
            Dd_Block[Index].sPhy = MDS.ddPp;
            Dd_Block[Index].sU = MDS.ddUp;
            Bac_block2.Bac_block2[TuneVariant].dU_cor[Pindex] = MDS.dUd;
            Bac_block2.Bac_block2[TuneVariant].dPhy_cor[Pindex] = MDS.dPd;
            FillModelRow(Index);
            ++Index;
            if (Index >= endcounter)
                Index = 0;
            memptr += MDSs;
        }
        FillHeaders();
        if (DNDialog(PovDev))
        {
            EMessageBox::information(this, "Отменено", "Операция отменена");
            return;
        }
        if (ConditionDataDialog())
        {
            EMessageBox::information(this, "Отменено", "Операция отменена");
            return;
        }
        GenerateReport();
    }
    else
    {
        EMessageBox::error(this, "Ошибка", "Неожиданный конец файла");
        return;
    }
}

#if PROGSIZE != PROGSIZE_EMUL
/*void A1Dialog::MeasTimerTimeout()
{
    if (Commands::GetBd(A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1)) == Error::ER_NOERROR)
        FillBdOut();
}*/

void A1Dialog::Accept()
{
//    float VoltageInkV, VoltageInV;
    int endcounter = (PovType == GOST_1983) ? 3 : 9;
    MeasurementTimer->stop();
    int Pindex = (Index > 4) ? (8 - Index) : Index;
    const int Percents23625[] = {20, 50, 80, 100, 120};
    const int Percents1983[] = {80, 100, 120};
    const int *Percents = (PovType == GOST_1983) ? Percents1983 : Percents23625;

    if (GetAndAverage(GAAT_BDA_OUT, &Dd_Block[Index]) == Error::ER_NOERROR)
    {
        FillMedian(Index);
        /*    // заполняем модель по полученным измерениям:
            if (GetStatistics() == Error::ER_GENERALERROR) // набираем статистику измерений и вычисляем средние значения
            {
                Cancelled = true;
                Decline();
                return;
            } */
        FillModelRow(Index);
        ++Index;
        if (Index >= endcounter)
        {
            if (StdFunc::FloatInRange(CurrentS, 0.25))
            {
                Index = 0;
                CurrentS = 1;
                if (EMessageBox::question(this, "Подтверждение", "На нагрузочном устройстве поверяемого ТН установите значение мощности, равное 1,0·Sном") == false)
                {
//                    Cancelled = true;
                    StdFunc::Cancel();
                    Decline();
                }
            }
            else
            {
                // запись файла протокола
                ReportHeader.PovDateTime = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");
                Commands::GetBd(A1_BDA_OUT_AN_BN, &ChA1->Bda_out_an, sizeof(CheckA1::A1_Bd4));
                ReportHeader.Freq = QString::number(ChA1->Bda_out.Frequency, 'g', 4);
                ReportHeader.Humidity = QString::number(ChA1->Bda_out_an.Hamb, 'g', 3);
                ReportHeader.Temp = QString::number(ChA1->Bda_out_an.Tamb, 'g', 2);
                if (DNDialog(PovDev)) // вводим данные по делителю
                {
                    EMessageBox::information(this, "Отменено", "Операция отменена");
                    return;
                }
                if (ConditionDataDialog()) // задаём условия поверки
                {
                    EMessageBox::information(this, "Отменено", "Операция отменена");
                    return;
                }
                GenerateReport();
                // вывод протокола на экран
                // формирование отчёта
                Decline();
                return;
            }
        }
        Pindex = (Index > 4) ? (8 - Index) : Index;
        if (ShowVoltageDialog(Percents[Pindex]) != Error::ER_NOERROR)
            Decline();
        return;
    }
    StdFunc::ClearCancel();
//    Cancelled = true;
    Decline();
    return;
}

void A1Dialog::Decline()
{
    WDFunc::SetEnabled(this, "cancelpb", false);
    WDFunc::SetEnabled(this, "acceptpb", false);
    WDFunc::SetEnabled(this, "StartWorkPb", true);
    MeasurementTimer->stop();
}

void A1Dialog::Proceed()
{
    PovType = TempPovType;
}
#endif

void A1Dialog::Cancel()
{
    WDFunc::SetEnabled(this, "StartWorkPb", true);
//    Cancelled = true;
    StdFunc::Cancel();
    emit Finished();
}

void A1Dialog::RBToggled()
{
    QString tmps = sender()->objectName();
    if (tmps == "rb1")
        TempPovType = GOST_23625;
    else
        TempPovType = GOST_1983;
}

#if PROGSIZE != PROGSIZE_EMUL
/*int A1Dialog::GetStatistics()
{
    // накопление измерений
    DdStruct tmpst2;
    tmpst2.dUrms = tmpst2.Phy = tmpst2.sPhy = tmpst2.sU = 0;
    QList<float> sPhy, sU;
    int count = 0;
    emit StartPercents(PovNumPoints);
    WaitWidget *w = new WaitWidget;
    w->SetMessage("Пожалуйста, подождите...");
    WaitWidget::ww_struct ww;
    ww.isincrement = true;
    ww.isallowedtostop = false;
    ww.format = WaitWidget::WW_SIMPLE;
    ww.initialseconds = 0;
    w->Init(ww);
    w->Start();
    while ((count < PovNumPoints) && !Cancelled)
    {
//        w->SetSeconds(PovNumPoints-count);

        if (Commands::GetBd(A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1)) == Error::ER_NOERROR)
            FillBdOut();
        else
        {
            w->close();
            EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda_out");
            return Error::ER_GENERALERROR;
        }
        tmpst2.dUrms += ChA1->Bda_out.dUrms;
        tmpst2.Phy += ChA1->Bda_out.Phy;
        sU.append(tmpst2.dUrms);
        sPhy.append(tmpst2.Phy);
        QTime tme;
        tme.start();
        while (tme.elapsed() < TUNE_POINTSPER)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        ++count;
        emit SetPercent(count);
    }
    w->close();
    if (Cancelled)
        return Error::ER_GENERALERROR;
    // усреднение
    float Um = tmpst2.dUrms / count; // среднее значение погрешности по напряжению
    float Phym = tmpst2.Phy / count; // среднее значение погрешности по углу
    Dd_Block.dUrms = Um;
    Dd_Block.Phy = Phym;
    float sUo, sPhyo; // временные накопительные СКО
    sUo = sPhyo = 0;
    for (int i=0; i<count; ++i)
    {
        if ((i < sPhy.size()) && (i < sU.size()))
        {
            sUo += qPow((sU.at(i) - Um), 2);
            sPhyo += qPow((sPhy.at(i) - Phym), 2);
        }
    }
    sUo = qSqrt(sUo/count);
    sPhyo = qSqrt(sPhyo/count);
    Dd_Block.sPhy = sPhyo;
    Dd_Block.sU = sUo; */
/*    FillMedian();
    return Error::ER_NOERROR;
}*/
#endif
void A1Dialog::TempRandomizeModel()
{
    // 1983
    PovType = GOST_1983;
    RepModel->SetSize(GOST1983ROWCOUNT, GOST1983COLCOUNT);
    for (int i=0; i<GOST1983ROWCOUNT; ++i)
    {
        for (int j=0; j<GOST1983COLCOUNT; ++j)
            RepModel->UpdateItem(i, j, static_cast<float>(qrand())/RAND_MAX, 5);
    }
    FillHeaders();
    GenerateReport();
}

int A1Dialog::ReadAnalogMeasurements()
{
    return Error::ER_NOERROR;
}

/*void A1Dialog::SetTuneParameters()
{
    if (!WDFunc::CBIndex(this, "tunevariantcb", TuneVariant))
        DBGMSG;
} */

/*void A1Dialog::InputTuneParameters(int varnum)
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QStringList sl;
    for (int i=0; i<varnum; ++i)
        sl << QString::number(i+1);
    hlyout->addWidget(WDFunc::NewLBLT(this, "Выберите вариант использования"), 0);
    QComboBox *cb = WDFunc::NewCB(this, "tunevariantcb", sl);
    cb->setMinimumWidth(50);
    hlyout->addWidget(cb, 0);
    lyout->addLayout(hlyout);
    QPushButton *pb = new QPushButton("Подтвердить");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetTuneParameters()));
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout = new QHBoxLayout;
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Cancel()));
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->exec();
} */

void A1Dialog::closeEvent(QCloseEvent *e)
{
    e->accept();
}
