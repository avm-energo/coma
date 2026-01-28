/* A1Dialog - dialog providing a main function of the device
 *
 */
#include "a1dialog.h"

#include "../gen/colors.h"
#include "../gen/commands.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/maindef.h"
#include "../gen/timefunc.h"
#include "../widgets/egroupbox.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include <gen/settings.h>
#include <gen/stdfunc.h>

#include <QCoreApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QStringListModel>
#include <QTableView>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>
#include <QtMath>

A1Dialog::A1Dialog(const QString &filename, QWidget *parent) : EAbstractTuneDialogA1DN(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ChA1 = new CheckA1;
    CA1 = new ConfigA1(S2Config);
    RepModel = new ReportModel;
    if (filename.isEmpty())
        SetupUI();
    else
    {
        ParsePKDNFile(filename);
        this->close();
        return;
    }
    TuneVariant = 0;
    // считать варианты использования и соответствующие им коэффициенты из модуля
    if (Commands::GetFile(1, S2Config) != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка чтения конфигурации из модуля");
        return;
    }
    if (GetBac() != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка чтения настроечных параметров из модуля");
        return;
    }
    WDFunc::SetLBLText(this, "tunevarcoef1", QString::number(Bac_block2.Bac_block2[0].K_DN, 'f', 0));
    WDFunc::SetLBLText(this, "tunevarcoef2", QString::number(Bac_block2.Bac_block2[1].K_DN, 'f', 0));
    WDFunc::SetLBLText(this, "tunevarcoef3", QString::number(Bac_block2.Bac_block2[2].K_DN, 'f', 0));
}

A1Dialog::~A1Dialog() { }

void A1Dialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    setStyleSheet(uconfWSS);
    QGridLayout *glyout = new QGridLayout;
    EGroupBox *gb = new EGroupBox("Конфигурация");
    glyout->addWidget(WDFunc::NewLBL(this, "Варианты использования:"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "1", "tunevar1", valuesLBLSS, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "2", "tunevar2", valuesLBLSS, ""), 0, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "3", "tunevar3", valuesLBLSS, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunevarcoef1", valuesLBLSS, ""), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunevarcoef2", valuesLBLSS, ""), 1, 2, 1, 1);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunevarcoef3", valuesLBLSS, ""), 1, 3, 1, 1);
    glyout->setColumnStretch(0, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    glyout = new QGridLayout;
    gb = new EGroupBox("Измерения в первичном масштабе");
    glyout->addWidget(WDFunc::NewLBL(this, "U1, кВ"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu1", valuesLBLSS, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "U2, кВ"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu2", valuesLBLSS, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "δU, %"), 0, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunepercent", valuesLBLSS, ""), 0, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Phy, мин"), 1, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednphy", valuesLBLSS, ""), 1, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Freq, Гц"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednfreq", valuesLBLSS, ""), 1, 1, 1, 1);
    QPushButton *pb = new QPushButton("Подтвердить");
    connect(pb, SIGNAL(clicked(bool)), this, SLOT(Good()));
    QHBoxLayout *hlyout = new QHBoxLayout;
    pb->setObjectName("acceptpb");
    pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked(bool)), this, SLOT(NoGood()));
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
    gb = new EGroupBox("СКО");
    glyout->addWidget(WDFunc::NewLBL(this, "dUrms(m), В"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunedurmsm", valuesLBLSS, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "φ(m), мин"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunephym", valuesLBLSS, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "σUrms, %"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunesurms", valuesLBLSS, ""), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "σφ, мин"), 1, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunesphy", valuesLBLSS, ""), 1, 3, 1, 1);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);

    pb = new QPushButton("Начать поверку делителя");
    pb->setObjectName("StartWorkPb");
    connect(pb, SIGNAL(clicked(bool)), this, SLOT(StartWork()));
    //    connect(pb,SIGNAL(clicked(bool)),this,SLOT(TempRandomizeModel()));
    lyout->addWidget(pb);
    lyout->addStretch(10);
    setLayout(lyout);
}

void A1Dialog::StartWork()
{
    TuneVariant = 0;
    Autonomous = false;
    StdFunc::ClearCancel();
    TemplateCheck();
    if (Commands::GetFile(1, S2Config) != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка чтения конфигурации из модуля");
        return;
    }
    // WDFunc::SetEnabled(this, "StartWorkPb", false);
    m_povType = m_tempPovType = GOST_NONE;
    if (GetBac() != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка чтения настроечных параметров из модуля");
        return;
    }
    InputTuneParameters(DNT_FOREIGN);
#ifndef AVM_DEBUG
    AndClearInitialValues();
    if (StdFunc::IsCancelled())
        return;
    if (Commands::SetUsingVariant(TuneVariant + 1) != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка установки варианта использования");
        return;
    }
    if (Commands::SetMode(m_mode) != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка установки рода напряжения");
        return;
    }
#endif
    QDialog *dlg = new QDialog(this);
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Выберите тип поверяемого оборудования"));
    QRadioButton *rb = new QRadioButton("Трансформаторы напряжения измерительные лабораторные по ГОСТ 23625-2001");
    connect(rb, &QRadioButton::toggled, this, [&]() { m_tempPovType = GOST_23625; });
    lyout->addWidget(rb);
    rb = new QRadioButton("Трансформаторы напряжения по ГОСТ 1983-2001");
    connect(rb, &QRadioButton::toggled, this, [&]() { m_tempPovType = GOST_1983; });
    lyout->addWidget(rb);
    rb = new QRadioButton("Делители напряжения");
    connect(rb, &QRadioButton::toggled, this, [&]() { m_tempPovType = DN; });
    lyout->addWidget(rb);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb, &QPushButton::clicked, this, [&]() { m_povType = m_tempPovType; });
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked(bool)), this, SLOT(Cancel()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->show();
    while ((m_povType == GOST_NONE) && !StdFunc::IsCancelled())
        TimeFunc::Wait();
    dlg->close();
    int rowcount = (m_povType == GOST_1983) ? GOST1983ROWCOUNT : GOST23625ROWCOUNT;
    int columncount = (m_povType == GOST_1983) ? GOST1983COLCOUNT : GOST23625COLCOUNT;
    RepModel->SetModel(rowcount, columncount);
    if (!StdFunc::IsCancelled())
    {
        if (EMessageBox::question(this, "Подтверждение",
                "Подключите вывод нижнего плеча \"своего\" делителя напряжения ко входу U1 прибора\n"
                "Вывод нижнего плеча поверяемого делителя или выход низшего напряжения поверяемого ТН(ДН) - ко входу "
                "U2\n"
                "На нагрузочном устройстве поверяемого ТН(ДН) установите значение мощности, равное 0,25·Sном")
            == true)
        {
            CurrentS = 0.25;
            m_index = 0;
            m_counter = 0;
            WDFunc::SetEnabled(this, "cancelpb", true);
            WDFunc::SetEnabled(this, "acceptpb", true);
            int percent = (m_povType == GOST_1983) ? 80 : 20;
            if (ShowVoltageDialog(percent) == Error::ER_NOERROR)
            {
                Go();
                return;
            }
        }
    }
    WDFunc::SetEnabled(this, "StartWorkPb", true);
    EMessageBox::information(this, "Информация", "Операция прервана");
}

int A1Dialog::GetConf()
{
    return Commands::GetFile(1, S2Config);
}

void A1Dialog::GenerateReport()
{
    Report *report;
    // данные в таблицу уже получены или из файла, или в процессе работы
    // отобразим таблицу
    FillHeaders();
    ShowTable();
    switch (m_povType)
    {
    case GOST_1983:
        report = new Report("a1_1983", this);
        break;
    case GOST_23625:
        report = new Report("a1_23625", this);
        break;
    default:
        report = new Report("a1_dn", this);
        break;
    }
    report->AddModel("maindata", RepModel);
    // запрос блока Bda_h, чтобы выдать KNI в протокол
#ifndef AVM_DEBUG
    if (m_mode == MODE_ALTERNATIVE)
    {
        if (!Autonomous)
        {
            if (Commands::GetBd(A1_BDA_H_BN, &ChA1->Bda_h, sizeof(CheckA1::A1_Bd2)) == Error::ER_NOERROR)
                report->SetVar("KNI", ChA1->Bda_h.HarmBuf[0][0], 5);
        }
        else
            report->SetVar("KNI", ChA1->Bda_h.HarmBuf[0][0], 5);
    }
    else
        report->SetVar("KNI", "");
#endif
    report->SetVar("Organization", Settings::get("OrganizationString", "Р&К"));
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

// TODO: implement DN branch

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
        PovDev.DevSN
            = QString::number(Results.DNFNum); // переписываем серийный номер установки для редактора полей протокола
        MainDataStruct MDS;
        int MDSCount;
        if (Results.GOST == 0)                 // GOST 1983
        {
            MDSCount = 6;
            rowcount = GOST1983ROWCOUNT;
            columncount = GOST1983COLCOUNT;
            m_povType = GOST_1983;
        }
        else if (Results.GOST == 1) // GOST 23625
        {
            MDSCount = 18;
            rowcount = GOST23625ROWCOUNT;
            columncount = GOST23625COLCOUNT;
            m_povType = GOST_23625;
        }
        else
        {
            EMessageBox::error(this, "Ошибка", "Ошибочный тип трансформатора в файле");
            return;
        }
        // разберём время
        quint32 tmpi = Results.Time >> 32;
        QDateTime tn = QDateTime::fromSecsSinceEpoch(tmpi, QTimeZone::UTC);
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
        int index = 0;
        m_DNType = DNT_FOREIGN;
        int endcounter = (m_povType == GOST_1983) ? 3 : 9;
        for (int i = 0; i < MDSCount; ++i)
        {
            int Pindex = (index > 4) ? (8 - index) : index;
            if (memptr > (ba.size() - MDSs))
            {
                EMessageBox::error(this, "Ошибка", "Неожиданный конец файла");
                return;
            }
            memcpy(&MDS, &ba.data()[memptr], MDSs);
            // заполним модель
            CurrentS = MDS.S;
            Dd_Block[index].dUrms = MDS.dUp;
            Dd_Block[index].Phy = MDS.dPp;
            Dd_Block[index].sPhy = MDS.ddPp;
            Dd_Block[index].sU = MDS.ddUp;
            Bac_block2.Bac_block2[TuneVariant].dU_cor[Pindex] = MDS.dUd;
            Bac_block2.Bac_block2[TuneVariant].dPhy_cor[Pindex] = MDS.dPd;
            FillModelRow(index);
            ++index;
            if (index >= endcounter)
                index = 0;
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

void A1Dialog::Go()
{
    int endcounter; // = (m_povType == GOST_1983) ? 3 : 9;
    QList<int> Percents;
    if (m_isSecVoltageIs100)
    {
        if (m_povType == GOST_1983)
        {
            Percents = { 50, 80, 100 };
            endcounter = 3;
        }
        else
        {
            Percents = { 20, 50, 80, 100 };
            endcounter = 7;
        }
    }
    else
    {
        if (m_povType == GOST_1983)
        {
            Percents = { 80, 100, 120 };
            endcounter = 3;
        }
        else
        {
            Percents = { 20, 50, 80, 100, 120 };
            endcounter = 9;
        }
    }

    while (true)
    {
        int res = GetAndAverage(GAAT_BDA_OUT, &Dd_Block[m_index], m_index);
        if (res == Error::ER_NOERROR)
        {
            FillMedian(m_index);
            ++m_index;
            if (m_index >= endcounter)
            {
                if (StdFunc::FloatIsWithinLimits(CurrentS, 0.25, TH01))
                {
                    m_index = 0;
                    CurrentS = 1;
                    if (EMessageBox::question(this, "Подтверждение",
                            "На нагрузочном устройстве поверяемого ТН установите значение мощности, равное "
                            "1,0·Sном")
                        == false)
                    {
                        StdFunc::Cancel();
                        Decline();
                        return;
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
            int Pindex = (m_index >= Percents.size()) ? (2 * Percents.size() - m_index - 2) : m_index;
            assert(Pindex < Percents.size());
            if (ShowVoltageDialog(Percents.at(Pindex)) != Error::ER_NOERROR)
            {
                Decline();
                return;
            }
        }
    }
    StdFunc::ClearCancel();
    Decline();
}

void A1Dialog::Decline()
{
    WDFunc::SetEnabled(this, "cancelpb", false);
    WDFunc::SetEnabled(this, "acceptpb", false);
    WDFunc::SetEnabled(this, "StartWorkPb", true);
    MeasurementTimer->stop();
}

void A1Dialog::Cancel()
{
    WDFunc::SetEnabled(this, "StartWorkPb", true);
    StdFunc::Cancel();
    emit Finished();
}

int A1Dialog::ReadAnalogMeasurements()
{
    return Error::ER_NOERROR;
}

void A1Dialog::closeEvent(QCloseEvent *e)
{
    e->accept();
}
