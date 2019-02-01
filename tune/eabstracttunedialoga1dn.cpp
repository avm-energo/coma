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
#include "../gen/commands.h"
#include "../widgets/wd_func.h"

EAbstractTuneDialogA1DN::EAbstractTuneDialogA1DN(QWidget *parent) :
    EAbstractTuneDialog (parent)
{

}

void EAbstractTuneDialogA1DN::InputTuneVariant(int varnum)
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
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Коэффициент деления для данного варианта: "));
    hlyout->addWidget(WDFunc::NewSPB(this, "kdnspb", 5, 1200, 0));
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Подтвердить");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetTuneVariant()));
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    Accepted = false;
    dlg->show();
    while (!Accepted && !StdFunc::IsCancelled())
        TimeFunc::Wait();
}

void EAbstractTuneDialogA1DN::SetTuneVariant()
{
    float kdn;
    if (!WDFunc::CBIndex(this, "tunevariantcb", TuneVariant))
        DBGMSG;
    if (!WDFunc::SPBData(this, "kdnspb", kdn))
        DBGMSG;
    if (Mode == MODE_ALTERNATIVE)
        Bac_block.Bac_block->K_DN = kdn;
    else
        Bac_block3.Bac_block3->K_DN = kdn;
    Accepted = true;
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

void EAbstractTuneDialogA1DN::GetBdAndFillMTT()
{
    if (Commands::GetBd(A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1)) == Error::ER_NOERROR)
        FillBdOut();
    if (Commands::GetBd(A1_BDA_IN_BN, &ChA1->Bda_in, sizeof(CheckA1::A1_Bd1)) == Error::ER_NOERROR)
        FillBdIn();
}

int EAbstractTuneDialogA1DN::GetAndAverage(int percent, int type, void *out)
{
    DdStruct *Dd;
    CheckA1::A1_Bd1 *Bd;

    if (type == GAAT_BDA_IN)
        Bd = static_cast<CheckA1::A1_Bd1 *>(out);
    else if (type == GAAT_BDA_OUT)
        Dd = static_cast<DdStruct *>(out);
    else
        return Error::ER_GENERALERROR;

    float VoltageInV = (Mode == MODE_ALTERNATIVE) ? (100 / qSqrt(3)) : (100 * qSqrt(2) / qSqrt(3));
    VoltageInV *= percent / 100;
    float VoltageInkV = (Mode == MODE_ALTERNATIVE) ? Bac_block.Bac_block[TuneVariant].K_DN : Bac_block3.Bac_block3[TuneVariant].K_DN;
    VoltageInkV *= VoltageInV / 1000;
    if (EMessageBox::question(this, "Подтверждение", "Подайте на делители напряжение " + \
                              QString::number(VoltageInkV, 'f', 1) + " кВ (" + QString::number(VoltageInV, 'f', 3) + " В) \n"
                              "и нажмите кнопку \"Продолжить\", когда напряжения установятся") == false)
    {
        StdFunc::Cancel();
        return Error::ER_GENERALERROR;
    }
    WaitNSeconds(WAITFORCONST);
    if (StartMeasurement() != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    FillBackBdOut();

    // накопление измерений
    DdStruct tmpdd;
    tmpdd.dUrms = tmpdd.Phy = tmpdd.sPhy = tmpdd.sU = 0;
    Bd->Phy = Bd->Uef_filt[0] = Bd->Uef_filt[1] = 0;
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
            Bd->Phy += ChA1->Bda_in.Phy;
            Bd->UefNat_filt[0] += ChA1->Bda_in.UefNat_filt[0];
            Bd->UefNat_filt[1] += ChA1->Bda_in.UefNat_filt[1];
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
        Bd->Phy /= count;
        Bd->Uef_filt[0] /= count;
        Bd->Uef_filt[1] /= count;
    }
    return Error::ER_NOERROR;
}

void EAbstractTuneDialogA1DN::FillBdOut()
{
    WDFunc::SetLBLText(this, "tunednu1", QString::number(ChA1->Bda_out.Uef_filt[0], 'f', 5));
    WDFunc::SetLBLText(this, "tunednu2", QString::number(ChA1->Bda_out.Uef_filt[1], 'f', 5));
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
    WDFunc::SetLBLText(this, "tunednu1i", QString::number(ChA1->Bda_in.Uef_filt[0], 'f', 5));
    WDFunc::SetLBLText(this, "tunednu2i", QString::number(ChA1->Bda_in.Uef_filt[1], 'f', 5));
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
