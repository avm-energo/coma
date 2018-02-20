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
#include "tunedialog22.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/commands.h"

TuneDialog22::TuneDialog22(int type, QWidget *parent) :
    EAbstractTuneDialog(parent)
{
    C22 = new Config22(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);
    BoardType = type;
    ChNum = 0;
    SetBac(&Bac_block, BoardType, sizeof(Bac_block));
    SetupUI();
}

QGroupBox * TuneDialog22::CoeffGB(const QString &title, const QString &coeff)
{
    int i;
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QGroupBox *gb = new QGroupBox(title);
    for (i = 0; i < AIN22_NUMCH; i++)
    {
        hlyout->addWidget(WDFunc::NewLBL(this, "k"+coeff+"["+QString::number(i)+"]:"));
        hlyout->addWidget(WDFunc::NewLE(this, "k"+coeff+"."+QString::number(i), "", ValuesLEFormat), 1);
        hlyout->addWidget(WDFunc::NewLBL(this, "b"+coeff+"["+QString::number(i)+"]:"));
        hlyout->addWidget(WDFunc::NewLE(this, "b"+coeff+"."+QString::number(i), "", ValuesLEFormat), 1);
        if ((i>0)&&!((i+1)%2))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }
    }
    if (hlyout->count())
        vlyout->addLayout(hlyout);
    gb->setLayout(vlyout);
    return gb;
}

void TuneDialog22::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *gb1lyout = new QVBoxLayout;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    int i;
    QWidget *tuneui = TuneUI();
    lyout->addWidget(tuneui);
    tuneui = new QWidget;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QStringList cbsl;
    for (i=0; i<AIN22_NUMCH; ++i)
        cbsl << QString::number(i);
    hlyout->addStretch(10);
    hlyout->addWidget(WDFunc::NewLBL(this, "Номер канала: "));
    hlyout->addWidget(WDFunc::NewCB(this, "tunenumch", cbsl), 2);
    QPushButton *pb = new QPushButton("Настроить");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(TuneOneChannel()));
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    lyout->addLayout(hlyout);
    QWidget *cp1 = new QWidget;
    cp1->setLayout(lyout);
    // cb + pb
    QWidget *cp2 = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->addTab(cp1,"Настройка");
    TuneTW->addTab(cp2,"Коэффициенты");
    lyout = new QVBoxLayout;
    gb1lyout->addWidget(CoeffGB("Настроечные коэффициенты при усилении 1x", "1"));
    gb1lyout->addWidget(CoeffGB("Настроечные коэффициенты при усилении 16x", "2"));
    gb1lyout->addWidget(BottomUI());
    lyout->addLayout(gb1lyout);
    cp2->setLayout(lyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
    if ((!(pc.ModuleBsi.Hth & HTH_REGPARS)) && !pc.Emul) // есть настроечные коэффициенты в памяти модуля
        ReadTuneCoefs(); // считать их из модуля и показать на экране
}

void TuneDialog22::SetLbls()
{
    lbls.append("1. Ввод пароля...");
    lbls.append("2. Отображение схемы подключения...");
    lbls.append("3. Регулировка...");
}

void TuneDialog22::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword; // 1. Ввод пароля
    int (EAbstractTuneDialog::*func)() = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialog22::ShowScheme);
    pf[lbls.at(count++)] = func; // 2. Отображение схемы подключения
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialog22::Tune);
    pf[lbls.at(count++)] = func; // 3. Регулировка
}

void TuneDialog22::FillBac()
{
    for (int j=0; j<COEFSNUM; ++j)
    {
        for (int i=0; i<AIN22_NUMCH; ++i)
        {
            WDFunc::SetLEData(this, "b"+QString::number(j)+"."+QString::number(i), QString::number(Bac_block[j].fbin[i], 'f', 5));
            WDFunc::SetLEData(this, "k"+QString::number(j)+"."+QString::number(i), QString::number(Bac_block[j].fkin[i], 'f', 5));
        }
    }
}

void TuneDialog22::FillBackBac()
{
    QString tmps;
    for (int j=0; j<COEFSNUM; ++j)
    {
        for (int i=0; i<AIN22_NUMCH; ++i)
        {
            WDFunc::LEData(this, "b"+QString::number(j)+"."+QString::number(i), tmps);
            Bac_block[j].fbin[i] = tmps.toFloat(&ok);
            if (ok)
            {
                WDFunc::LEData(this, "k"+QString::number(j)+"."+QString::number(i), tmps);
                Bac_block[i].fkin[i] = tmps.toFloat(&ok);
                if (ok)
                    continue;
            }
            WARNMSG("Ошибка при переводе во float");
        }
    }
}

void TuneDialog22::GetBdAndFillMTT()
{

}

int TuneDialog22::ShowScheme()
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
    if (pc.Cancelled == true)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialog22::Show0(int coef)
{
    if (coef < 2)
    {
        if (QMessageBox::information(this,"Настройка",\
                                     "Задайте значение сопротивления "+QString::number(R0[coef], 'f', 2) + \
                                     " Ом\nна входе "+QString::number(ChNum)+\
                                     " модуля и нажмите OK") == QMessageBox::Ok)
            return NOERROR;
    }
    return GENERALERROR;
}

int TuneDialog22::ShowW100(int coef)
{
    if (coef < 2)
    {
        float R100 = R0[coef]*C22->Bci_block.W100[ChNum];
        if (QMessageBox::information(this,"Настройка",\
                                     "Задайте значение сопротивления"+QString::number(R100) + \
                                     " Ом\nна входе "+QString::number(ChNum)+\
                                     " модуля и нажмите OK") == QMessageBox::Ok)
            return NOERROR;
    }
    return GENERALERROR;
}

int TuneDialog22::TuneChannel(int Type)
{
    switch (Type)
    {
    case TTUNE_0: // настройка нуля
    {
        if (Commands::GetBda(BoardType, &Bda0, sizeof(Check22::Bda)) == NOERROR)
        {
            CheckAndShowTune0();
            return NOERROR;
        }
        return GENERALERROR;
    }
    case TTUNE_W100: // настройка W100
    {
        if (Commands::GetBda(BoardType, &BdaW100, sizeof(Check22::Bda)) == NOERROR)
        {
            CheckAndShowTuneW100();
            return NOERROR;
        }
        return GENERALERROR;
    }
    default:
        break;
    }
    return GENERALERROR;
}

int TuneDialog22::Tune()
{
    for (ChNum=0; ChNum<AIN22_NUMCH; ++ChNum)
    {
        if (TuneOneChannel() == GENERALERROR)
            return GENERALERROR;
    }
    return NOERROR;
}

bool TuneDialog22::CheckAndShowTune0()
{
    WDFunc::SetLBLText(this, "tunech"+QString::number(ChNum), QString::number(Bda0.sin[ChNum]));
    if (!CalcNewTuneCoef())
        return false;
    FillBac();
    return true;
}

bool TuneDialog22::CheckAndShowTuneW100()
{
    WDFunc::SetLBLText(this, "tunech"+QString::number(ChNum), QString::number(BdaW100.sin[ChNum]));
    if (!CalcNewTuneCoef())
        return false;
    FillBac();
    return true;
}

bool TuneDialog22::CalcNewTuneCoef()
{
    double Rk = 1.2 / (1.5 * 0.0002 * 8388608);
    double R0 = C22->Bci_block.RzeroT[ChNum];
    double R100 = C22->Bci_block.W100[ChNum] * C22->Bci_block.RzeroT[ChNum];
    if (Bda0.sin[ChNum] == BdaW100.sin[ChNum])
    {
        WARNMSG("Ошибка в настроечных коэффициентах, деление на ноль");
        return false;
    }
    Bac_block[CoefNum].fbin[ChNum] = R0 - Bda0.sin[ChNum] * (R100 - R0) / (BdaW100.sin[ChNum] - Bda0.sin[ChNum]);
    Bac_block[CoefNum].fkin[ChNum] = (R100 - R0) / (Rk * (BdaW100.sin[ChNum] - Bda0.sin[ChNum]));
    return true;
}

void TuneDialog22::SetDefCoefs()
{
    for (int j = 0; j < COEFSNUM; ++j)
    {
        for (int i=0; i<AIN22_NUMCH; i++)
        {
            Bac_block[j].fbin[i] = 0.0;
            Bac_block[j].fkin[i] = 1.0;
        }
    }
    FillBac();
}

int TuneDialog22::ReadAnalogMeasurements()
{
    return NOERROR;
}

int TuneDialog22::TuneOneChannel()
{
    for (CoefNum=0; CoefNum<2; ++CoefNum)
    {
        WDFunc::CBIndex(this, "tunenumch", ChNum);
        Show0(CoefNum);
        if (TuneChannel(TTUNE_0) != NOERROR)
            return GENERALERROR;
        ShowW100(CoefNum);
        if (TuneChannel(TTUNE_W100) != NOERROR)
            return GENERALERROR;
    }
    return NOERROR;
}

bool TuneDialog22::CheckTuneCoefs()
{
    for (int j=0; j<COEFSNUM; ++j)
    {
        for (int i=0; i<AIN22_NUMCH; i++)
        {
            float tmpf;
            if (!WDFunc::LEData(this, "k"+QString::number(j)+"."+QString::number(i), tmpf))
                return false;
            if (!WDFunc::LEData(this, "b"+QString::number(j)+"."+QString::number(i), tmpf))
                return false;
        }
    }
    return true;
}
