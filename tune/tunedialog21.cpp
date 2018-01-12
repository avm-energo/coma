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
#include "../gen/commands.h"

TuneDialog21::TuneDialog21(int type, QWidget *parent) :
    EAbstractTuneDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    for (int i = 0; i < 16; i++)
    {
        Bda0.sin[i] = 0;
        Bda5.sin[i] = UINT_MAX;
        Bda20.sin[i] = UINT_MAX;
    }
    BoardType = type;
    ChNum = 0;
    SetBac(&Bac_block, BoardType, sizeof(Bac_block));
    SetupUI();
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
        le->setObjectName("tunebcoef"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        gb2lyout->addWidget(le, 1);
        lbl = new QLabel("u"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        le = new QLineEdit("");
        le->setObjectName("tunek1coef"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        gb2lyout->addWidget(le, 1);
        lbl = new QLabel("i"+QString::number(i)+":");
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
    gb1lyout->addWidget(BottomUI());
/*    QPushButton *pb = new QPushButton("Установить настроечные коэффициенты по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetDefCoefs()));
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Прочитать настроечные коэффициенты из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в модуль");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    pb->setIcon(QIcon("images/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFromFile()));
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setIcon(QIcon("images/save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    gb1lyout->addWidget(pb); */
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);
    cp2->setLayout(lyout);

/*    QGridLayout *glyout = new QGridLayout;
    lbl = new QLabel("Канал");
    glyout->addWidget(lbl,0,0,1,1,Qt::AlignCenter);
    lbl = new QLabel("Значение");
    glyout->addWidget(lbl,0,1,1,1,Qt::AlignCenter);
    lbl = new QLabel("Настройка нуля");
    glyout->addWidget(lbl,0,2,1,1,Qt::AlignCenter);
    lbl = new QLabel("Настройка 20 мА");
    glyout->addWidget(lbl,0,3,1,1,Qt::AlignCenter);
    lbl = new QLabel("Настройка 5 В");
    glyout->addWidget(lbl,0,4,1,1,Qt::AlignCenter);
    glyout->setColumnStretch(0, 0);
    glyout->setColumnStretch(1, 3);
    glyout->setColumnStretch(2, 1);
    glyout->setColumnStretch(3, 1);
    glyout->setColumnStretch(4, 1);
    for (i = 0; i < AIN21_NUMCH; i++)
    {
        lbl = new QLabel(QString::number(i));
        glyout->addWidget(lbl,i+1,0,1,1,Qt::AlignCenter);
        lbl = new QLabel("");
        lbl->setObjectName("tunech"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,i+1,1,1,1);
        pb = new QPushButton("Запуск");
        pb->setObjectName("0"+QString::number(i));
        connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
        if (pc.Emul)
            pb->setEnabled(false);
        glyout->addWidget(pb,i+1,2,1,1,Qt::AlignCenter);
        pb = new QPushButton("Запуск");
        pb->setObjectName("1"+QString::number(i));
        connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
        if (pc.Emul)
            pb->setEnabled(false);
        glyout->addWidget(pb,i+1,3,1,1,Qt::AlignCenter);
        pb = new QPushButton("Запуск");
        pb->setObjectName("2"+QString::number(i));
        connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
        if (pc.Emul)
            pb->setEnabled(false);
        glyout->addWidget(pb,i+1,4,1,1,Qt::AlignCenter);
    }
    cp3->setLayout(glyout); */
    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
    if ((!(pc.ModuleBsi.Hth & HTH_REGPARS)) && !pc.Emul) // есть настроечные коэффициенты в памяти модуля
        ReadTuneCoefs(); // считать их из модуля и показать на экране
}

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
    int (EAbstractTuneDialog::*func)() = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialog21::ShowScheme);
    pf[lbls.at(count++)] = func; // 2. Отображение схемы подключения
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialog21::Tune);
    pf[lbls.at(count++)] = func; // 3. Регулировка
}

void TuneDialog21::FillBac()
{
    for (int i=0; i<AIN21_NUMCH; ++i)
    {
        WDFunc::SetLEData(this, "tunebcoef"+QString::number(i), QString::number(Bac_block[i].fbin, 'f', 5));
        WDFunc::SetLEData(this, "tunek1coef"+QString::number(i), QString::number(Bac_block[i].fkiin, 'f', 5));
        WDFunc::SetLEData(this, "tunek2coef"+QString::number(i), QString::number(Bac_block[i].fkuin, 'f', 5));
    }
}

void TuneDialog21::FillBackBac()
{
    QString tmps;
    for (int i=0; i<AIN21_NUMCH; ++i)
    {
        WDFunc::LEData(this, "tunebcoef"+QString::number(i), tmps);
        Bac_block[i].fbin = tmps.toFloat(&ok);
        if (ok)
        {
            WDFunc::LEData(this, "tunek1coef"+QString::number(i), tmps);
            Bac_block[i].fkiin = tmps.toFloat(&ok);
            if (ok)
            {
                WDFunc::LEData(this, "tunek2coef"+QString::number(i), tmps);
                Bac_block[i].fkuin = tmps.toFloat(&ok);
                if (ok)
                    continue;
            }
        }
        WARNMSG("Ошибка при переводе во float");
    }
}

void TuneDialog21::GetBdAndFillMTT()
{

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
    if (pc.Cancelled == true)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialog21::ShowU0()
{
    if (QMessageBox::information(this,"Настройка",\
                                 "На калибраторе задайте напряжение 0 В на\nвходе "+\
                                 QString::number(ChNum)+" модуля и нажмите OK") == QMessageBox::Ok)
        return NOERROR;
    return GENERALERROR;
}

int TuneDialog21::ShowI20()
{
    if (QMessageBox::information(this,"Настройка",\
                                 "Переключите входные переключатели на ток,\nустановите ток 20 мА на\n" \
                                 "входе " + QString::number(ChNum) + " модуля и нажмите OK") == QMessageBox::Ok)
        return NOERROR;
    return GENERALERROR;
}

int TuneDialog21::ShowU5()
{
    if (QMessageBox::information(this,"Настройка",\
                                 "Переключите входные переключатели на напряжение,\nустановите напряжение" \
                                 "5 В на \nвходе " + QString::number(ChNum) + " модуля и нажмите OK") == QMessageBox::Ok)
        return NOERROR;
    return GENERALERROR;
}

int TuneDialog21::TuneChannel(int Type)
{
    switch (Type)
    {
    case ATUNE_U0: // настройка нуля
    {
        if (Commands::GetBda(BoardType, &Bda0, sizeof(Check21::Bda)) == NOERROR)
        {
            CheckAndShowTune0();
            return NOERROR;
        }
        return GENERALERROR;
    }
    case ATUNE_I20: // настройка 20 мА
    {
        if (Commands::GetBda(BoardType, &Bda20, sizeof(Check21::Bda)) == NOERROR)
        {
            CheckAndShowTune20();
            return NOERROR;
        }
        return GENERALERROR;
    }
    case ATUNE_U5: // настройка 5 В
    {
        if (Commands::GetBda(BoardType, &Bda5, sizeof(Check21::Bda)) == NOERROR)
        {
            CheckAndShowTune5();
            return NOERROR;
        }
        return GENERALERROR;
    }
    default:
        break;
    }
    return GENERALERROR;
}

int TuneDialog21::Tune()
{
    for (ChNum=0; ChNum<AIN21_NUMCH; ++ChNum)
    {
        ShowU0();
        if (!TuneChannel(ATUNE_U0))
            return GENERALERROR;
        ShowI20();
        if (!TuneChannel(ATUNE_I20))
            return GENERALERROR;
    }
    for (ChNum=0; ChNum<AIN21_NUMCH; ++ChNum)
    {
        ShowU5();
        if (TuneChannel(ATUNE_U5) != NOERROR)
            return GENERALERROR;
    }
    return NOERROR;
}

bool TuneDialog21::CheckAndShowTune0()
{
    WDFunc::SetLBLText(this, "tunech"+QString::number(ChNum), QString::number(Bda0.sin[ChNum]/ATUNENUMPOINTS,16));
    if (!CalcNewTuneCoef())
        return false;
    FillBac();
    return true;
}

bool TuneDialog21::CheckAndShowTune5()
{
    WDFunc::SetLBLText(this, "tunech"+QString::number(ChNum), QString::number(Bda5.sin[ChNum]/ATUNENUMPOINTS,16));
    if (!CalcNewTuneCoef())
        return false;
    FillBac();
    return true;
}

bool TuneDialog21::CheckAndShowTune20()
{
    WDFunc::SetLBLText(this, "tunech"+QString::number(ChNum), QString::number(Bda20.sin[ChNum]/ATUNENUMPOINTS,16));
    if (!CalcNewTuneCoef())
        return false;
    FillBac();
    return true;
}

bool TuneDialog21::CalcNewTuneCoef()
{
    Bac_block[ChNum].fbin = 1.25 - (static_cast<float>(Bda0.sin[ChNum]) / (ATUNENUMPOINTS*1638.0));
    if ((Bda0.sin[ChNum] == Bda5.sin[ChNum]) || (Bda0.sin[ChNum] == Bda20.sin[ChNum]))
    {
        WARNMSG("Ошибка в настроечных коэффициентах, деление на ноль");
        return false;
    }
    Bac_block[ChNum].fkuin = ATUNENUMPOINTS*1638.0 / static_cast<float>(Bda0.sin[ChNum]-Bda5.sin[ChNum]);
    Bac_block[ChNum].fkiin = ATUNENUMPOINTS*1638.0 / static_cast<float>(Bda0.sin[ChNum]-Bda20.sin[ChNum]);
    return true;
}

void TuneDialog21::SetDefCoefs()
{
    for (int i=0; i<AIN21_NUMCH; i++)
    {
        Bac_block[i].fbin = 0.0;
        Bac_block[i].fkiin = 1.0;
        Bac_block[i].fkuin = 1.0;
    }
    FillBac();
}

int TuneDialog21::ReadAnalogMeasurements()
{
    return NOERROR;
}

void TuneDialog21::TuneOneChannel()
{
    WDFunc::CBIndex(this, "tunenumch", ChNum);
    ShowU0();
    if (TuneChannel(ATUNE_U0) != NOERROR)
        return;
    ShowI20();
    if (TuneChannel(ATUNE_I20) != NOERROR)
        return;
    ShowU5();
    if (TuneChannel(ATUNE_U5) != NOERROR)
        return;
}

bool TuneDialog21::CheckTuneCoefs()
{
    for (int i=0; i<AIN21_NUMCH; i++)
    {
        int tmpi;
        if (!WDFunc::LEData(this, "tunebcoef"+QString::number(i), tmpi))
            return false;
        if (!WDFunc::LEData(this, "tunek1coef"+QString::number(i), tmpi))
            return false;
        if (!WDFunc::LEData(this, "tunek2coef"+QString::number(i), tmpi))
            return false;
    }
    return true;
}
