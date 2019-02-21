#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QCoreApplication>
#include "tunedialog21.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/colors.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"
#include "../gen/maindef.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

TuneDialog21::TuneDialog21(BoardTypes type, QWidget *parent) :
    EAbstractTuneDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    for (int i = 0; i < 16; i++)
    {
        BdaU0.sin[i] = 0;
        BdaI0.sin[i] = 0;
        Bda5.sin[i] = 1.0;
        Bda20.sin[i] = 1.0;
    }
    BoardType = type;
    SetBac(&Bac_block, BoardType, sizeof(Bac_block));
    SetupUI();
//#if PROGSIZE != PROGSIZE_EMUL
//    PrereadConf();
//#endif
}

TuneDialog21::~TuneDialog21()
{

}

void TuneDialog21::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *gb1lyout = new QVBoxLayout;
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *tuneui = TuneUI();
    lyout->addWidget(tuneui);
    tuneui = new QWidget;
#if PROGSIZE != PROGSIZE_EMUL
    QHBoxLayout *hlyout = new QHBoxLayout;
    QStringList cbsl;
    for (i=0; i<AIN21_NUMCH; ++i)
        cbsl << QString::number(i);
    hlyout->addStretch(10);
    hlyout->addWidget(WDFunc::NewLBL(this, "Номер канала: "));
    hlyout->addWidget(WDFunc::NewCB(this, "tunenumch", cbsl), 2);
    QPushButton *pb = new QPushButton("Настроить");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(TuneOneChannel()));
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    lyout->addLayout(hlyout);
#endif
    QWidget *cp1 = new QWidget;
    cp1->setLayout(lyout);
    // cb + pb
    QWidget *cp2 = new QWidget;
//    QWidget *cp3 = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    QLabel *lbl;

    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->addTab(cp1,"Настройка");
    TuneTW->addTab(cp2,"Коэффициенты");
//    TuneTW->addTab(cp3,"Настройка");
    lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < AIN21_NUMCH; i++)
    {
        lbl = new QLabel("b"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        QLineEdit *le = new QLineEdit("");
        le->setObjectName("tunebcoefI"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        gb2lyout->addWidget(le, 1);
        le = new QLineEdit("");
        le->setObjectName("tunebcoefU"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        gb2lyout->addWidget(le, 1);
        lbl = new QLabel("i"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        le = new QLineEdit("");
        le->setObjectName("tunek1coef"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        gb2lyout->addWidget(le, 1);
        lbl = new QLabel("u"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        le = new QLineEdit("");
        le->setObjectName("tunek2coef"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        gb2lyout->addWidget(le, 1);
        if ((i>0)&&!((i+1)%2))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);
    gb1lyout->addWidget(BottomUI(BoardType));
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);
    cp2->setLayout(lyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
#if PROGSIZE != PROGSIZE_EMUL
    if ((!(ModuleBSI::GetHealth() & HTH_REGPARS)) && !StdFunc::IsInEmulateMode()) // есть настроечные коэффициенты в памяти модуля
        ReadTuneCoefsByBac(0); // считать их из модуля и показать на экране
#endif
}

#if PROGSIZE != PROGSIZE_EMUL
void TuneDialog21::SetLbls()
{
    lbls.append("1. Ввод пароля...");
    lbls.append("2. Отображение схемы подключения...");
    lbls.append("3. Регулировка...");
}

void TuneDialog21::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword; // 1. Ввод пароля
    int (EAbstractTuneDialog::*func)() = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog21::ShowScheme);
    pf[lbls.at(count++)] = func; // 2. Отображение схемы подключения
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog21::Tune);
    pf[lbls.at(count++)] = func; // 3. Регулировка
}

int TuneDialog21::ShowScheme()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap("images/tune21.png")));
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
    if (StdFunc::IsCancelled() == true)
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialog21::ShowU0(int ChNum)
{
    if (EMessageBox::question(this,"Настройка",\
                                 "Включите на модуле режим измерения напряжений\n"
                                 "На калибраторе задайте напряжение 0 В на\nвходе "+\
                                 QString::number(ChNum)+" модуля и нажмите OK", nullptr, "Ok" , "Close"))
        return Error::ER_NOERROR;
    else
        return Error::ER_GENERALERROR;
}

int TuneDialog21::ShowI0(int ChNum)
{
    if (EMessageBox::question(this,"Настройка",\
                                 "Включите на модуле режим измерения токов\n"
                                 "На калибраторе задайте ток 0 мА на\nвходе "+\
                                 QString::number(ChNum)+" модуля и нажмите OK", nullptr, "Ok" , "Close"))
        return Error::ER_NOERROR;
    else
        return Error::ER_GENERALERROR;
}

int TuneDialog21::ShowI20(int ChNum)
{
    if (EMessageBox::question(this,"Настройка",\
                                 "Переключите входные переключатели на ток,\nустановите ток 20 мА на\n" \
                                 "входе " + QString::number(ChNum) + " модуля и нажмите OK", nullptr, "Ok" , "Close"))
        return Error::ER_NOERROR;
        else
        return Error::ER_GENERALERROR;
}

int TuneDialog21::ShowU5(int ChNum)
{
    if (EMessageBox::question(this,"Настройка",\
                                 "Переключите входные переключатели на напряжение,\nустановите напряжение" \
                                 "5 В на \nвходе " + QString::number(ChNum) + " модуля и нажмите OK", nullptr, "Ok" , "Close"))
        return Error::ER_NOERROR;
        else
        return Error::ER_GENERALERROR;
}


int TuneDialog21::TuneChannel(Check21::Bda &Bda)
{
    return Commands::GetBda(BoardType, &Bda, sizeof(Check21::Bda));
}

int TuneDialog21::Tune()
{
    int i;
    for (i=0; i<AIN21_NUMCH; ++i)
    {
        if(ShowU0(i) == Error::ER_GENERALERROR)
           return Error::ER_GENERALERROR;
        if (TuneChannel(BdaU0) != Error::ER_NOERROR)
            return Error::ER_GENERALERROR;
        if(ShowI0(i) == Error::ER_GENERALERROR)
           return Error::ER_GENERALERROR;
        if (TuneChannel(BdaI0) != Error::ER_NOERROR)
            return Error::ER_GENERALERROR;
    }
    for (i=0; i<AIN21_NUMCH; ++i)
    {
        if(ShowI20(i) == Error::ER_GENERALERROR)
            return Error::ER_GENERALERROR;
        if (TuneChannel(Bda20) != Error::ER_NOERROR)
            return Error::ER_GENERALERROR;
    }
    for (i=0; i<AIN21_NUMCH; ++i)
    {
        if(ShowU5(i) == Error::ER_GENERALERROR)
            return Error::ER_GENERALERROR;
        if (TuneChannel(Bda5) != Error::ER_NOERROR)
            return Error::ER_GENERALERROR;
        if (!CalcNewTuneCoef(i))
            return Error::ER_GENERALERROR;
    }
    return Error::ER_NOERROR;
}

bool TuneDialog21::CalcNewTuneCoef(int NumCh)
{
    Bac_block.Bac1[NumCh].fbin_I = 1.25f - BdaI0.sin[NumCh];
    Bac_block.Bac2[NumCh].fbin_U  = 1.25f - BdaU0.sin[NumCh];
    if (StdFunc::FloatInRange(BdaU0.sin[NumCh], Bda5.sin[NumCh]) || StdFunc::FloatInRange(BdaI0.sin[NumCh], Bda20.sin[NumCh]))
    {
        WARNMSG("Ошибка в настроечных коэффициентах, деление на ноль");
        return false;
    }
    Bac_block.Bac1[NumCh].fkuin = 1 / (BdaU0.sin[NumCh]-Bda5.sin[NumCh]);
    Bac_block.Bac1[NumCh].fkiin = 1 / (BdaI0.sin[NumCh]-Bda20.sin[NumCh]);
    FillBac(0);
    return true;
}

int TuneDialog21::ReadAnalogMeasurements()
{
    return Error::ER_NOERROR;
}

void TuneDialog21::TuneOneChannel()
{
    int NumCh = 0;
    NumCh = WDFunc::CBIndex(this, "tunenumch");
    if (ShowU0(NumCh) == Error::ER_GENERALERROR)
        return;
    if (TuneChannel(BdaU0) != Error::ER_NOERROR)
        return;
    if (ShowI0(NumCh) == Error::ER_GENERALERROR)
        return;
    if (TuneChannel(BdaI0) != Error::ER_NOERROR)
        return;
    if(ShowI20(NumCh) == Error::ER_GENERALERROR)
        return;
    if (TuneChannel(Bda20) != Error::ER_NOERROR)
        return;
    if(ShowU5(NumCh) == Error::ER_GENERALERROR)
        return;
    if (TuneChannel(Bda5) != Error::ER_NOERROR)
        return;
    CalcNewTuneCoef(NumCh);
    FillBac(0);
}

void TuneDialog21::Cancel()
{
    emit close();

}


bool TuneDialog21::CheckTuneCoefs()
{
    for (int i=0; i<AIN21_NUMCH; i++)
    {
        int tmpi = 0;
        if (!WDFunc::LEData(this, "tunebcoefI"+QString::number(i), tmpi))
            return false;
        if (!WDFunc::LEData(this, "tunebcoefU"+QString::number(i), tmpi))
            return false;
        if (!WDFunc::LEData(this, "tunek1coef"+QString::number(i), tmpi))
            return false;
        if (!WDFunc::LEData(this, "tunek2coef"+QString::number(i), tmpi))
            return false;
    }
    return true;
}

void TuneDialog21::GetBdAndFillMTT()
{

}

#endif

void TuneDialog21::FillBac(int bacnum)
{
    Q_UNUSED(bacnum);
    for (int i=0; i<AIN21_NUMCH; ++i)
    {
        WDFunc::SetLEData(this, "tunebcoefI"+QString::number(i), QString::number(Bac_block.Bac1[i].fbin_I, 'f', 5));
        WDFunc::SetLEData(this, "tunebcoefU"+QString::number(i), QString::number(Bac_block.Bac2[i].fbin_U, 'f', 5));
        WDFunc::SetLEData(this, "tunek1coef"+QString::number(i), QString::number(Bac_block.Bac1[i].fkiin, 'f', 5));
        WDFunc::SetLEData(this, "tunek2coef"+QString::number(i), QString::number(Bac_block.Bac1[i].fkuin, 'f', 5));
    }
}

void TuneDialog21::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum);
    QString tmps;
    for (int i=0; i<AIN21_NUMCH; ++i)
    {
        WDFunc::LEData(this, "tunebcoefI"+QString::number(i), tmps);
        Bac_block.Bac1[i].fbin_I = tmps.toFloat(&ok);
        WDFunc::LEData(this, "tunebcoefU"+QString::number(i), tmps);
        Bac_block.Bac2[i].fbin_U = tmps.toFloat(&ok);
        if (ok)
        {
            WDFunc::LEData(this, "tunek1coef"+QString::number(i), tmps);
            Bac_block.Bac1[i].fkiin = tmps.toFloat(&ok);
            if (ok)
            {
                WDFunc::LEData(this, "tunek2coef"+QString::number(i), tmps);
                Bac_block.Bac1[i].fkuin = tmps.toFloat(&ok);
                if (ok)
                    continue;
            }
        }
        WARNMSG("Ошибка при переводе во float");
    }
}

void TuneDialog21::SetDefCoefs()
{
    for (int i=0; i<AIN21_NUMCH; i++)
    {
        Bac_block.Bac1[i].fbin_I = 0.0;
        Bac_block.Bac2[i].fbin_U = 0.0;
        Bac_block.Bac1[i].fkiin = 1.0;
        Bac_block.Bac1[i].fkuin = 1.0;
    }
    FillBac(BoardType);
}
