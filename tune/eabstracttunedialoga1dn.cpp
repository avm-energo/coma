#include <QtMath>
#include <QComboBox>
#include <QLayout>
#include <QPushButton>
#include "eabstracttunedialoga1dn.h"
#include "../widgets/emessagebox.h"
#include "../widgets/waitwidget.h"
#include "../gen/timefunc.h"
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif
#include "../widgets/wd_func.h"

EAbstractTuneDialogA1DN::EAbstractTuneDialogA1DN(QWidget *parent) :
    EAbstractTuneDialog (parent)
{

}

void EAbstractTuneDialogA1DN::InputTuneParameters(int dntype)
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Заводской номер ТН(ДН)"), 0);
    hlyout->addWidget(WDFunc::NewLE(this, "dnfnumle", QString::number(Bac_block2.DNFNum)), 10);
    lyout->addLayout(hlyout);
    DNType = dntype;
    QStringList sl;
    for (int i=0; i<TUNEVARIANTSNUM; ++i)
        sl << QString::number(i+1);
    hlyout->addWidget(WDFunc::NewLBLT(this, "Вариант включения делителя"), 0);
    hlyout->addWidget(WDFunc::NewCB(this, "tunevariantcb", sl), 10);
    WDFunc::CBConnect(this, "tunevariantcb", WDFunc::CT_INDEXCHANGED, this, SLOT(SetKDNByTuneVariant(int)));
    lyout->addLayout(hlyout);
    if (DNType == DNT_OWN)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(this, "Коэффициент деления для данного варианта: "));
        hlyout->addWidget(WDFunc::NewSPB(this, "kdnspb", 5, 99999, 0), 10);
        lyout->addLayout(hlyout);
    }
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLT(this, "Род напряжения"), 0);
    sl = QStringList() << "Переменное" << "Постоянное";
    hlyout->addWidget(WDFunc::NewCB(this, "voltagetype", sl), 10);
#if PROGSIZE != PROGSIZE_EMUL
    if (Commands::GetMode(Mode) == Error::ER_NOERROR)
        WDFunc::SetCBIndex(this, "voltagetype", Mode);
#endif
    lyout->addLayout(hlyout);
    QPushButton *pb = new QPushButton("Подтвердить");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetTuneParameters()));
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    Accepted = false;
    WDFunc::SetSPBData(this, "kdnspb", Bac_block2.Bac_block2[WDFunc::CBIndex(this, "tunevariantcb")].K_DN);
    dlg->show();
    while (!Accepted && !StdFunc::IsCancelled())
        TimeFunc::Wait();
}

void EAbstractTuneDialogA1DN::SetTuneParameters()
{
    float kdn;
    if ((TuneVariant = WDFunc::CBIndex(this, "tunevariantcb")) == -1)
    {
        DBGMSG;
        return;
    }
    if ((VoltageType = WDFunc::CBIndex(this, "voltagetype")) == -1)
    {
        DBGMSG;
        return;
    }
    Mode = VoltageType;
    if (DNType == DNT_OWN)
    {
        if (!WDFunc::SPBData(this, "kdnspb", kdn))
            DBGMSG;
        if (Mode == MODE_ALTERNATIVE)
            Bac_block2.Bac_block2[TuneVariant].K_DN = kdn;
        else
            Bac_block3.Bac_block3[TuneVariant].K_DN = kdn;
        if (Mode == MODE_ALTERNATIVE)
            WDFunc::LENumber(this, "dnfnumle", Bac_block2.DNFNum);
        else
            WDFunc::LENumber(this, "dnfnumle", Bac_block3.DNFNum);
    }
    Accepted = true;
}

bool EAbstractTuneDialogA1DN::ConditionDataDialog()
{
    StdFunc::ClearCancel();
//    Cancelled = false;
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
    if (StdFunc::FloatInRange(ChA1->Bda_out_an.Tamb, FLT_MAX, TH1))
        tmps = "";
    else
        tmps = ReportHeader.Temp;
    glyout->addWidget(WDFunc::NewLE(this, "Temp", tmps), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Влажность воздуха, %"), row, 0, 1, 1, Qt::AlignRight);
    if (StdFunc::FloatInRange(ChA1->Bda_out_an.Hamb, FLT_MAX, TH1))
        tmps = "";
    else
        tmps = ReportHeader.Humidity;
    glyout->addWidget(WDFunc::NewLE(this, "Humidity", tmps), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Атмосферное давление, кПа"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "Pressure", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Напряжение питания сети, В"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "Voltage", ""), row++, 1, 1, 1, Qt::AlignLeft);
/*    glyout->addWidget(WDFunc::NewLBL(this, "Частота питания сети, Гц"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "Frequency", ReportHeader.Freq), row++, 1, 1, 1, Qt::AlignLeft); */
    glyout->setColumnStretch(1, 1);
    lyout->addLayout(glyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetConditionData()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(CancelTune()));
    lyout->addWidget(pb);
    connect(this,SIGNAL(Finished()),dlg,SLOT(close()));
    dlg->setLayout(lyout);
    dlg->exec();
    return StdFunc::IsCancelled();
}

bool EAbstractTuneDialogA1DN::DNDialog(PovDevStruct &PovDev)
{
    QString DNNamePhaseLblText = (DNType == DNT_FOREIGN) ? "Обозначение по схеме, фаза" : "Номер ступени делителя";
    QString DNNamePhaseLeText = (DNType == DNT_FOREIGN) ? "" : QString::number(TuneVariant);
    StdFunc::ClearCancel();
//    Cancelled = false;
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
    glyout->addWidget(WDFunc::NewLBL(this, DNNamePhaseLblText), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNNamePhase", DNNamePhaseLeText), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Заводской номер"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNSerialNum", ReportHeader.DNSerNum), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Класс точности, %"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNTolerance", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное первичное напряжение, кВ"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNU1", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение, В"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNU2", ""), row++, 1, 1, 1, Qt::AlignLeft);
    if (DNType == DNT_OWN)
    {
        glyout->addWidget(WDFunc::NewLBL(this, "Номинальная мощность нагрузки, ВА"), row, 0, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLE(this, "DNP", ""), row++, 1, 1, 1, Qt::AlignLeft);
    }
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
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(CancelTune()));
    lyout->addWidget(pb);
    connect(this,SIGNAL(Finished()),dlg,SLOT(close()));
    dlg->setLayout(lyout);
    dlg->exec();
    return StdFunc::IsCancelled();
}

void EAbstractTuneDialogA1DN::SetDNData()
{
//    QString PovDev, PovDevSN, PovDevPrecision;
    QString tmps;
    WDFunc::LEData(this, "UKDNOrganization", tmps);
    StdFunc::SetOrganizationString(tmps);
    WDFunc::LEData(this, "DNType", ReportHeader.DNType);
    WDFunc::LEData(this, "DNNamePhase", ReportHeader.DNNamePhase);
    WDFunc::LEData(this, "DNSerialNum", ReportHeader.DNSerNum);
    WDFunc::LEData(this, "DNTolerance", ReportHeader.DNTol);
    WDFunc::LEData(this, "DNU1", ReportHeader.DNU1);
    WDFunc::LEData(this, "DNU2", ReportHeader.DNU2);
    WDFunc::LEData(this, "DNP", ReportHeader.DNP);
    WDFunc::LEData(this, "DNFreq", ReportHeader.DNF);
    WDFunc::LEData(this, "DNOrganization", ReportHeader.DNOrganization);
    WDFunc::LEData(this, "DNPlace", ReportHeader.DNPlace);
    WDFunc::LEData(this, "DNInspection", ReportHeader.OuterInsp);
    WDFunc::LEData(this, "DNWindingInspection", ReportHeader.WindingsInsp);
    WDFunc::LEData(this, "PovDev", PovDev.DevName);
    WDFunc::LEData(this, "PovDevSN", PovDev.DevSN);
    WDFunc::LEData(this, "PovDevPrecision", PovDev.DevPrecision);
    ReportHeader.DNDevices = PovDev.DevName + " сер. номер " + PovDev.DevSN + ", кл. точн. " + PovDev.DevPrecision;
    SaveSettings();
    emit Finished();
}

void EAbstractTuneDialogA1DN::SetConditionData()
{
    WDFunc::LEData(this, "Temp", ReportHeader.Temp);
    WDFunc::LEData(this, "Humidity", ReportHeader.Humidity);
    WDFunc::LEData(this, "Pressure", ReportHeader.Pressure);
    WDFunc::LEData(this, "Voltage", ReportHeader.Voltage);
//    WDFunc::LEData(this, "Frequency", ReportHeader.Freq);
    emit Finished();
}

void EAbstractTuneDialogA1DN::FillModelRow(int index)
{
    int row;
    const int Percents23625[] = {20, 50, 80, 100, 120};
    const int Percents1983[] = {80, 100, 120};
    const int *Percents = (PovType == GOST_1983) ? Percents1983 : Percents23625;

    if (DNType == DNT_OWN) // для собственного ДН PovType = GOST_23625 и проценты идут задом наперёд
        row = (index > 4) ? (index - 4) : (4 - index);
    else
        row = (index > 4) ? (8 - index) : index;

    int percent = Percents[row];
    float sign = (DNType == DNT_OWN) ? -1.0 : 1.0;
    // заполняем модель по полученным измерениям:
    if ((StdFunc::FloatInRange(CurrentS, 1)) && (DNType == DNT_FOREIGN))
    {
        if (PovType == GOST_1983)
            row += GOST1983ROWCOUNT/2;
        else
            row += GOST23625ROWCOUNT/2;
    }
    if (index <= 4) // на нисходящем отрезке по ГОСТ 23625
    {
        int column = 0;
        RepModel->UpdateItem(row, column++, percent, 0);

        if (DNType == DNT_OWN)
            RepModel->UpdateItem(row, column++, "");
        else
            RepModel->UpdateItem(row, column++, CurrentS,  3);

        RepModel->UpdateItem(row, column++, Dd_Block[index].dUrms*sign, 5);

        if (Mode == MODE_ALTERNATIVE)
            RepModel->UpdateItem(row, column++, Dd_Block[index].Phy*sign, 5);
        else
            RepModel->UpdateItem(row, column++, "");

        if (PovType == GOST_23625)
            column += 12;
        else
        {
            if (DNType == DNT_FOREIGN)
                RepModel->UpdateItem(row, column++, Bac_block2.Bac_block2[TuneVariant].dU_cor[index]*sign, 5);
            else
                RepModel->UpdateItem(row, column++, Dd_Block[index].dUrms*sign, 5);

            if (Mode == MODE_ALTERNATIVE)
            {
                if (DNType == DNT_FOREIGN)
                    RepModel->UpdateItem(row, column++, Bac_block2.Bac_block2[TuneVariant].dPhy_cor[index]*sign, 5);
                else
                    RepModel->UpdateItem(row, column++, Dd_Block[index].Phy*sign, 5);
            }
            else
                RepModel->UpdateItem(row, column++, "");

            RepModel->UpdateItem(row, column++, Dd_Block[index].dUrms*sign, 5);

            if (Mode == MODE_ALTERNATIVE)
                RepModel->UpdateItem(row, column++, Dd_Block[index].Phy*sign, 5);
            else
                RepModel->UpdateItem(row, column++, "");
        }
        RepModel->UpdateItem(row, column++, Dd_Block[index].sU, 5);

        if (Mode == MODE_ALTERNATIVE)
            RepModel->UpdateItem(row, column++, Dd_Block[index].sPhy, 5);
        else
            RepModel->UpdateItem(row, column++, "");
    }
    if (index >= 4)
    {
        RepModel->UpdateItem(row, 4, Dd_Block[index].dUrms*sign, 5);
        float dUrmsU = RepModel->Item(row, 2);
        float UrmsM = (dUrmsU + Dd_Block[index].dUrms*sign) / 2;
        RepModel->UpdateItem(row, 6, UrmsM, 5);
        if (DNType == DNT_FOREIGN)
            RepModel->UpdateItem(row, 8, Bac_block2.Bac_block2[TuneVariant].dU_cor[8-index]*sign, 5);
        else
            RepModel->UpdateItem(row, 8, UrmsM, 5);
        RepModel->UpdateItem(row, 10, (Dd_Block[index].dUrms*sign - dUrmsU), 5);
        RepModel->UpdateItem(row, 12, UrmsM, 5);
        RepModel->UpdateItem(row, 14, Dd_Block[index].sU, 5);
        if (Mode == MODE_ALTERNATIVE)
        {
            RepModel->UpdateItem(row, 5, Dd_Block[index].Phy*sign, 5);
            float PhyU = RepModel->Item(row, 3);
            float PhyM = (PhyU + Dd_Block[index].Phy*sign) / 2;
            RepModel->UpdateItem(row, 7, PhyM, 5);
            if (DNType == DNT_FOREIGN)
                RepModel->UpdateItem(row, 9, Bac_block2.Bac_block2[TuneVariant].dPhy_cor[8-index]*sign, 5);
            else
                RepModel->UpdateItem(row, 9, PhyM, 5);
            RepModel->UpdateItem(row, 11, (Dd_Block[index].Phy*sign - PhyU), 5);
            RepModel->UpdateItem(row, 13, PhyM, 5);
            RepModel->UpdateItem(row, 15, Dd_Block[index].sPhy, 5);
        }
        else
        {
            RepModel->UpdateItem(row, 5, "");
            RepModel->UpdateItem(row, 7, "");
            RepModel->UpdateItem(row, 9, "");
            RepModel->UpdateItem(row, 11, "");
            RepModel->UpdateItem(row, 13, "");
            RepModel->UpdateItem(row, 15, "");
        }
    }
}

void EAbstractTuneDialogA1DN::FillHeaders()
{
    QStringList sl = QStringList() << "Проц" << "S/Sном" << "dUrms(u)" << "Phy(u)";
    if (PovType == GOST_23625)
        sl << "dUrms(d)" << "Phy(d)" << "dUrms(ud)" << "Phy(ud)" << "dUrms(md)" << "Phy(md)" << \
              "dUrms(u-d)" << "Phy(u-d)" << "dUrms" << "Phy" << "sUrms(d)" << "sPhy(d)";
    else
        sl << "dUcor" << "dPhycor" << "dUrms" << "Phy";
    sl << "sUrms(u)" << "sPhy(u)";
    RepModel->SetHeader(sl);
}

void EAbstractTuneDialogA1DN::TemplateCheck()
{
    QString GOST = (PovType == GOST_1983) ? "1983" : "23625";
    QString path = StdFunc::GetSystemHomeDir()+"a1_"+GOST+".lrxml";
    QFile file(path);
    if (!file.exists()) // нет файла шаблона
    {
        QString tmps = "Файл шаблона не найден по указанному пути:" + path + \
                "Выходной протокол не будет сформирован. Рекомендуется переустановка ПКДН-Сервис";
        EMessageBox::information(this, "Предупреждение", tmps);
    }
    LoadSettings();
}

void EAbstractTuneDialogA1DN::FillBac(int bacnum)
{
    if (bacnum == BLOCK_ALTERNATIVE)
    {
        for (int j=0; j<TUNEVARIANTSNUM; ++j)
        {
            QString Si = QString::number(j);
            WDFunc::SetSPBData(this, "K_DNSPB."+Si, Bac_block2.Bac_block2[j].K_DN);
            for (int i = 0; i < 6; ++i)
            {
                WDFunc::SetLEData(this, "tune"+QString::number(i+4)+"."+Si, QString::number(Bac_block2.Bac_block2[j].U1kDN[i], 'f', 5));
                WDFunc::SetLEData(this, "tune"+QString::number(i+10)+"."+Si, QString::number(Bac_block2.Bac_block2[j].U2kDN[i], 'f', 5));
                WDFunc::SetLEData(this, "tune"+QString::number(i+16)+"."+Si, QString::number(Bac_block2.Bac_block2[j].PhyDN[i], 'f', 5));
                if (i < 5)
                {
                    WDFunc::SetLEData(this, "tune"+QString::number(i+22)+"."+Si, QString::number(Bac_block2.Bac_block2[j].dU_cor[i], 'f', 5));
                    WDFunc::SetLEData(this, "tune"+QString::number(i+27)+"."+Si, QString::number(Bac_block2.Bac_block2[j].dPhy_cor[i], 'f', 5));
                    WDFunc::SetLEData(this, "tune"+QString::number(i+32)+"."+Si, QString::number(Bac_block2.Bac_block2[j].ddU_cor[i], 'f', 5));
                    WDFunc::SetLEData(this, "tune"+QString::number(i+37)+"."+Si, QString::number(Bac_block2.Bac_block2[j].ddPhy_cor[i], 'f', 5));
                }
            }
            WDFunc::SetLEData(this, "DividerSN"+Si, QString::number(Bac_block2.DNFNum));
        }
    }
    else
    {
        for (int j=0; j<TUNEVARIANTSNUM; ++j)
        {
            QString Si = QString::number(j+3);
            WDFunc::SetSPBData(this, "K_DNSPB."+Si, Bac_block3.Bac_block3[j].K_DN);
            for (int i = 0; i < 6; ++i)
            {
                WDFunc::SetLEData(this, "tune"+QString::number(i+4)+"."+Si, QString::number(Bac_block3.Bac_block3[j].U1kDN[i], 'f', 5));
                WDFunc::SetLEData(this, "tune"+QString::number(i+10)+"."+Si, QString::number(Bac_block3.Bac_block3[j].U2kDN[i], 'f', 5));
                if (i < 5)
                {
                    WDFunc::SetLEData(this, "tune"+QString::number(i+22)+"."+Si, QString::number(Bac_block3.Bac_block3[j].dU_cor[i], 'f', 5));
                    WDFunc::SetLEData(this, "tune"+QString::number(i+32)+"."+Si, QString::number(Bac_block3.Bac_block3[j].ddU_cor[i], 'f', 5));
                }
            }
            WDFunc::SetLEData(this, "DividerSN"+Si, QString::number(Bac_block3.DNFNum));
        }
    }
}

void EAbstractTuneDialogA1DN::FillBackBac(int bacnum)
{
    QString tmps;
    if (bacnum == BLOCK_ALTERNATIVE)
    {
        for (int j=0; j<TUNEVARIANTSNUM; ++j)
        {
            QString Si = QString::number(j);
            WDFunc::SPBData(this, "K_DNSPB."+Si, Bac_block2.Bac_block2[j].K_DN);
            for (int i = 0; i < 6; i++)
            {
                WDFunc::LENumber(this, "tune"+QString::number(i+4)+"."+Si, Bac_block2.Bac_block2[j].U1kDN[i]);
                WDFunc::LENumber(this, "tune"+QString::number(i+10)+"."+Si, Bac_block2.Bac_block2[j].U2kDN[i]);
                WDFunc::LENumber(this, "tune"+QString::number(i+16)+"."+Si, Bac_block2.Bac_block2[j].PhyDN[i]);
                if (i < 5)
                {
                    WDFunc::LENumber(this, "tune"+QString::number(i+22)+"."+Si, Bac_block2.Bac_block2[j].dU_cor[i]);
                    WDFunc::LENumber(this, "tune"+QString::number(i+27)+"."+Si, Bac_block2.Bac_block2[j].dPhy_cor[i]);
                    WDFunc::LENumber(this, "tune"+QString::number(i+32)+"."+Si, Bac_block2.Bac_block2[j].ddU_cor[i]);
                    WDFunc::LENumber(this, "tune"+QString::number(i+37)+"."+Si, Bac_block2.Bac_block2[j].ddPhy_cor[i]);
                }
            }
            WDFunc::LEData(this, "DividerSN"+Si, tmps);
            Bac_block2.DNFNum = tmps.toInt();
        }
    }
    else
    {
        for (int j=0; j<TUNEVARIANTSNUM; ++j)
        {
            QString Si = QString::number(j+3);
            WDFunc::SPBData(this, "K_DNSPB."+Si, Bac_block3.Bac_block3[j].K_DN);
            for (int i = 0; i < 6; i++)
            {
                WDFunc::LENumber(this, "tune"+QString::number(i+4)+"."+Si, Bac_block3.Bac_block3[j].U1kDN[i]);
                WDFunc::LENumber(this, "tune"+QString::number(i+10)+"."+Si, Bac_block3.Bac_block3[j].U2kDN[i]);
                if (i < 5)
                {
                    WDFunc::LENumber(this, "tune"+QString::number(i+22)+"."+Si, Bac_block3.Bac_block3[j].dU_cor[i]);
                    WDFunc::LENumber(this, "tune"+QString::number(i+32)+"."+Si, Bac_block3.Bac_block3[j].ddU_cor[i]);
                }
            }
            WDFunc::LEData(this, "DividerSN"+Si, tmps);
            Bac_block3.DNFNum = tmps.toInt();
        }
    }
}

void EAbstractTuneDialogA1DN::SetDefCoefs()
{
    int bacnum = sender()->objectName().toInt();
    float defu2[6] = {0.0, 12.0, 30.0, 48.0, 60.0, 72.0};
    float defu3[6] = {0.0, 16.0, 40.0, 64.0, 80.0, 100.0};

    if (bacnum == 2)
    {
        for (int i=0; i<TUNEVARIANTSNUM; ++i)
        {
            for (int j=0; j<6; ++j)
            {
                Bac_block2.Bac_block2[i].U1kDN[j] = defu2[j];
                Bac_block2.Bac_block2[i].U2kDN[j] = defu2[j];
                Bac_block2.Bac_block2[i].PhyDN[j] = 0.0;
                if (j < 5)
                {
                    Bac_block2.Bac_block2[i].dPhy_cor[j] = 0.0;
                    Bac_block2.Bac_block2[i].dU_cor[j] = 0.0;
                    Bac_block2.Bac_block2[i].ddPhy_cor[j] = 0.0;
                    Bac_block2.Bac_block2[i].ddU_cor[j] = 0.0;
                }
            }
        }
        Bac_block2.Bac_block2[0].K_DN = 350.0;
        Bac_block2.Bac_block2[1].K_DN = 1100.0;
        Bac_block2.Bac_block2[2].K_DN = 2200.0;
        Bac_block2.DNFNum = 1;
    }
    if (bacnum == 3)
    {
        for (int i=0; i<TUNEVARIANTSNUM; ++i)
        {
            for (int j=0; j<6; ++j)
            {
                Bac_block3.Bac_block3[i].U1kDN[j] = defu3[j];
                Bac_block3.Bac_block3[i].U2kDN[j] = defu3[j];
                if (j < 5)
                {
                    Bac_block3.Bac_block3[i].dU_cor[j] = 0.0;
                    Bac_block3.Bac_block3[i].ddU_cor[j] = 0.0;
                }
            }
        }
        Bac_block3.Bac_block3[0].K_DN = 350.0;
        Bac_block3.Bac_block3[1].K_DN = 1100.0;
        Bac_block3.Bac_block3[2].K_DN = 2200.0;
        Bac_block3.DNFNum = 1;
    }
    FillBac(bacnum);
}

void EAbstractTuneDialogA1DN::LoadSettings()
{
    QSettings *sets = new QSettings ("EvelSoft", PROGNAME);
    PovDev.DevName = sets->value("PovDevName", "UPTN").toString();
    PovDev.DevSN = sets->value("PovDevSN", "00000001").toString();
    PovDev.DevPrecision = sets->value("PovDevPrecision", "0.05").toString();
    PovNumPoints = sets->value("PovNumPoints", "60").toInt();
    if ((PovNumPoints <= 0) || (PovNumPoints > 1000))
        PovNumPoints = 60;
//    OrganizationString = sets->value("Organization", "Р&К").toString();
}

void EAbstractTuneDialogA1DN::SaveSettings()
{
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    sets->setValue("PovDevName", PovDev.DevName);
    sets->setValue("PovDevSN", PovDev.DevSN);
    sets->setValue("PovDevPrecision", PovDev.DevPrecision);
    sets->setValue("PovNumPoints", QString::number(PovNumPoints, 10));
//    sets->setValue("Organization", OrganizationString);
}

#if PROGSIZE != PROGSIZE_EMUL
void EAbstractTuneDialogA1DN::GetBdAndFillMTT()
{
    if (Commands::GetBd(A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1)) == Error::ER_NOERROR)
        FillBdOut();
    if (Commands::GetBd(A1_BDA_IN_BN, &ChA1->Bda_in, sizeof(CheckA1::A1_Bd1)) == Error::ER_NOERROR)
        FillBdIn();
}

int EAbstractTuneDialogA1DN::GetAndAverage(int type, void *out, int index)
{
    DdStruct *Dd;
    CheckA1::A1_Bd1 *Bd;

    if (type == GAAT_BDA_IN)
        Bd = static_cast<CheckA1::A1_Bd1 *>(out);
    else if (type == GAAT_BDA_OUT)
        Dd = static_cast<DdStruct *>(out);
    else
        return Error::ER_GENERALERROR;

    WaitNSeconds(WAITFORCONST);
    if (StartMeasurement() != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    FillBackBdOut();

    // накопление измерений
    DdStruct tmpdd;
    CheckA1::A1_Bd1 tmpbd;
    tmpdd.dUrms = tmpdd.Phy = tmpdd.sPhy = tmpdd.sU = 0;
    tmpbd.Phy = tmpbd.UefNat_filt[0] = tmpbd.UefNat_filt[1] = 0;
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
    while ((count < PovNumPoints) && !StdFunc::IsCancelled())
    {
        if (type == GAAT_BDA_OUT)
        {
            if (Commands::GetBd(A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1)) == Error::ER_NOERROR)
                FillBdOut();
            else
            {
                w->close();
                EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda_out");
                return Error::ER_GENERALERROR;
            }
            tmpdd.dUrms += ChA1->Bda_out.dUrms;
            tmpdd.Phy += ChA1->Bda_out.Phy;
            sU.append(ChA1->Bda_out.dUrms);
            sPhy.append(ChA1->Bda_out.Phy);
        }
        else
        {
            if (Commands::GetBd(A1_BDA_IN_BN, &ChA1->Bda_in, sizeof(CheckA1::A1_Bd1)) == Error::ER_NOERROR)
                FillBdIn();
            else
            {
                w->close();
                EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda_in");
                return Error::ER_GENERALERROR;
            }
            tmpbd.Phy += ChA1->Bda_in.Phy;
            tmpbd.UefNat_filt[0] += ChA1->Bda_in.UefNat_filt[0];
            tmpbd.UefNat_filt[1] += ChA1->Bda_in.UefNat_filt[1];
        }
        QTime tme;
        tme.start();
        while (tme.elapsed() < TUNE_POINTSPER)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        ++count;
        emit SetPercent(count);
    }
    w->close();
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    // усреднение
    if (type == GAAT_BDA_OUT)
    {
        float Um = tmpdd.dUrms / count; // среднее значение погрешности по напряжению
        if (!StdFunc::FloatInRange(Um, 0, TH005))
        {
            EMessageBox::error(this, "Ошибка", "Среднее значение погрешности за пределами 0,05 %");
            return Error::ER_GENERALERROR;
        }
        float Phym = tmpdd.Phy / count; // среднее значение погрешности по углу
        Dd->dUrms = Um;
        Dd->Phy = Phym;
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
        if (!StdFunc::FloatInRange(sUo, 0, TH005) || !StdFunc::FloatInRange(sPhyo, 0, TH05))
        {
            EMessageBox::error(this, "Ошибка", "Среднее значение погрешности за пределами 0,05 % или по углу - за 0,5");
            return Error::ER_GENERALERROR;
        }
        Dd->sPhy = sPhyo;
        Dd->sU = sUo;
    }
    else
    {
        Bd->Phy = tmpbd.Phy / count;
        Bd->UefNat_filt[0] = tmpbd.UefNat_filt[0] / count;
        Bd->UefNat_filt[1] = tmpbd.UefNat_filt[1] / count;
    }
    FillModelRow(index);
    return Error::ER_NOERROR;
}

int EAbstractTuneDialogA1DN::GetBac()
{
    if (Commands::GetBac(2, &Bac_block2, sizeof(Bac2)) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    if (Commands::GetBac(3, &Bac_block3, sizeof(Bac3)) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int EAbstractTuneDialogA1DN::AndClearInitialValues()
{
    if ((TuneVariant > 2) || (TuneVariant < 0))
        return Error::ER_GENERALERROR;
    Bac_block2.Bac_block2[TuneVariant].U1kDN[0] = 0;
    Bac_block2.Bac_block2[TuneVariant].U2kDN[0] = 0;
    Bac_block2.Bac_block2[TuneVariant].PhyDN[0] = 0;
    Bac_block3.Bac_block3[TuneVariant].U1kDN[0] = 0;
    Bac_block3.Bac_block3[TuneVariant].U2kDN[0] = 0;
    return Error::ER_NOERROR;
}
#endif

int EAbstractTuneDialogA1DN::ShowVoltageDialog(int percent)
{
    float VoltageInV = (Mode == MODE_ALTERNATIVE) ? (100 / qSqrt(3)) : (100 * qSqrt(2) / qSqrt(3));
    VoltageInV *= static_cast<float>(percent) / 100.0;
    float VoltageInkV = (Mode == MODE_ALTERNATIVE) ? Bac_block2.Bac_block2[TuneVariant].K_DN : Bac_block3.Bac_block3[TuneVariant].K_DN;
    VoltageInkV *= VoltageInV / 1000.0;
    if (EMessageBox::question(this, "Подтверждение", "Подайте на делители напряжение " + \
                              QString::number(VoltageInkV, 'f', 1) + " кВ (" + QString::number(VoltageInV, 'f', 3) + " В) \n"
                              "и нажмите кнопку \"Продолжить\", когда напряжения установятся") == false)
    {
        StdFunc::Cancel();
        return Error::ER_GENERALERROR;
    }
    return Error::ER_NOERROR;
}

void EAbstractTuneDialogA1DN::FillBdOut()
{
    WDFunc::SetLBLText(this, "tunednu1", QString::number(ChA1->Bda_out.UefNat_filt[0], 'f', 5));
    WDFunc::SetLBLText(this, "tunednu2", QString::number(ChA1->Bda_out.UefNat_filt[1], 'f', 5));
    WDFunc::SetLBLText(this, "tunednphy", QString::number(ChA1->Bda_out.Phy, 'f', 5));
    WDFunc::SetLBLText(this, "tunednfreq", QString::number(ChA1->Bda_out.Frequency, 'f', 5));
    WDFunc::SetLBLText(this, "tunepercent", QString::number(ChA1->Bda_out.dUrms, 'f', 5));
}

void EAbstractTuneDialogA1DN::FillBackBdOut()
{
    QString tmps;
    WDFunc::LBLText(this, "tunednu1", tmps);
    ChA1->Bda_out.Uef_filt[0] = tmps.toFloat();
    WDFunc::LBLText(this, "tunednu2", tmps);
    ChA1->Bda_out.Uef_filt[1] = tmps.toFloat();
    WDFunc::LBLText(this, "tunednphy", tmps);
    ChA1->Bda_out.Phy = tmps.toFloat();
    WDFunc::LBLText(this, "tunednfreq", tmps);
    ChA1->Bda_out.Frequency = tmps.toFloat();
    WDFunc::LBLText(this, "tunepercent", tmps);
    ChA1->Bda_out.dUrms = tmps.toFloat();
}

void EAbstractTuneDialogA1DN::FillBdIn()
{
    WDFunc::SetLBLText(this, "tunednu1i", QString::number(ChA1->Bda_in.UefNat_filt[0], 'f', 5));
    WDFunc::SetLBLText(this, "tunednu2i", QString::number(ChA1->Bda_in.UefNat_filt[1], 'f', 5));
    WDFunc::SetLBLText(this, "tunednphyi", QString::number(ChA1->Bda_in.Phy, 'f', 5));
    WDFunc::SetLBLText(this, "tunednfreqi", QString::number(ChA1->Bda_in.Frequency, 'f', 5));
    WDFunc::SetLBLText(this, "tunepercenti", QString::number(ChA1->Bda_in.dUrms, 'f', 5));
}

void EAbstractTuneDialogA1DN::FillBackBdIn()
{
    QString tmps;
    WDFunc::LBLText(this, "tunednu1i", tmps);
    ChA1->Bda_in.Uef_filt[0] = tmps.toFloat();
    WDFunc::LBLText(this, "tunednu2i", tmps);
    ChA1->Bda_in.Uef_filt[1] = tmps.toFloat();
    WDFunc::LBLText(this, "tunednphyi", tmps);
    ChA1->Bda_in.Phy = tmps.toFloat();
    WDFunc::LBLText(this, "tunednfreqi", tmps);
    ChA1->Bda_in.Frequency = tmps.toFloat();
    WDFunc::LBLText(this, "tunepercenti", tmps);
    ChA1->Bda_in.dUrms = tmps.toFloat();
}

void EAbstractTuneDialogA1DN::FillMedian(int index)
{
    if (index < TUNEA1LEVELS)
    {
        WDFunc::SetLBLText(this, "tunedurmsm", QString::number(Dd_Block[index].dUrms, 'f', 5));
        WDFunc::SetLBLText(this, "tunephym", QString::number(Dd_Block[index].Phy, 'f', 5));
        WDFunc::SetLBLText(this, "tunesurms", QString::number(Dd_Block[index].sU, 'f', 5));
        WDFunc::SetLBLText(this, "tunesphy", QString::number(Dd_Block[index].sPhy, 'f', 5));
    }
}

void EAbstractTuneDialogA1DN::TempRandomizeModel()
{
    // 1983
    PovType = GOST_23625;
    RepModel->SetModel(GOST23625ROWCOUNT, GOST23625COLCOUNT);
    for (int i=0; i<GOST23625ROWCOUNT; ++i)
    {
        for (int j=0; j<GOST23625COLCOUNT; ++j)
            RepModel->UpdateItem(i, j, static_cast<float>(qrand())/RAND_MAX, 5);
    }
    FillHeaders();
}

void EAbstractTuneDialogA1DN::SetKDNByTuneVariant(int index)
{
    if (index < 3)
        WDFunc::SetSPBData(this, "kdnspb", Bac_block2.Bac_block2[index].K_DN);
}
