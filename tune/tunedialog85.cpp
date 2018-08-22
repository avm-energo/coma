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
#include "tunedialog85.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/stdfunc.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/maindef.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif


void TuneDialog85::SetupUI()
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QWidget *cp1 = TuneUI();
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl;
    QGridLayout *glyout = new QGridLayout;

    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->addTab(cp1,"Настройка");
    TuneTW->addTab(cp2,"Коэффициенты");
    TuneTW->addTab(cp3,"Данные МИП");

    // CP2 - КОЭФФИЦИЕНТЫ МОДУЛЯ

    lyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < 3; i++)
    {
        lbl = new QLabel("KmU["+QString::number(i)+"]");
        glyout->addWidget(lbl,0,i,1,1);
        QLineEdit *le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,1,i,1,1);
        lbl = new QLabel("KmI_1["+QString::number(i)+"]");
        glyout->addWidget(lbl,2,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+3));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,3,i,1,1);
        lbl = new QLabel("KmI_4["+QString::number(i)+"]");
        glyout->addWidget(lbl,4,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+6));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,5,i,1,1);
        lbl = new QLabel("KmU2["+QString::number(i)+"]");
        glyout->addWidget(lbl,6,i,1,1);
        le = new QLineEdit("");
        le->setObjectName("tune"+QString::number(i+9));
        le->setStyleSheet(ValuesLEFormat);
        glyout->addWidget(le,7,i,1,1);
    }
    lbl=new QLabel("K_freq:");
    lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,8,0,1,1);
    QLineEdit *le = new QLineEdit("");
    le->setObjectName("tune13");
    le->setStyleSheet(ValuesLEFormat);
    glyout->addWidget(le,8,1,1,2);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addWidget(BottomUI());
    lyout->addStretch(1);
    cp2->setLayout(lyout);

    // CP3 - ПОКАЗАНИЯ МИП-02

    QVBoxLayout *vlyout = new QVBoxLayout;
    QVBoxLayout *gblyout = new QVBoxLayout;
    gb = new QGroupBox("Измеряемые параметры");
    QHBoxLayout *hlyout = MipPars(1, "Частота");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(4, "Фазное напряжение");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(7, "Фазный ток");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(11, "Угол нагрузки");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(14, "Фазовый угол напряжения");
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    lbl = new QLabel("10. Ток N");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip10");
    lbl->setToolTip("Параметр 10");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    lbl = new QLabel("17. Темп.");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip17");
    lbl->setToolTip("Параметр 17");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    gblyout->addLayout(hlyout);
    gb->setLayout(gblyout);
    vlyout->addWidget(gb);

    gb = new QGroupBox("Вычисляемые параметры");
    hlyout = MipPars(22, "Активная мощность");
    gblyout = new QVBoxLayout;
    gblyout->addLayout(hlyout);
    hlyout = MipPars(26, "Реактивная мощность");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(30, "Полная мощность");
    gblyout->addLayout(hlyout);
    hlyout = MipPars(19, "Линейное напряжение");
    gblyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    lbl = new QLabel("25. Акт:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip25");
    lbl->setToolTip("Параметр 25");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    lbl = new QLabel("29. Реакт:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip29");
    lbl->setToolTip("Параметр 29");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    gblyout->addLayout(hlyout);
    hlyout=new QHBoxLayout;
    lbl = new QLabel("33. Полная:");
    hlyout->addWidget(lbl);
    lbl = new QLabel("");
    lbl->setObjectName("mip33");
    lbl->setToolTip("Параметр 33");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl,10);
    gblyout->addLayout(hlyout);
    gb->setLayout(gblyout);
    vlyout->addWidget(gb);
#if PROGSIZE != PROGSIZE_EMUL
    hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Запустить связь с МИП");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMip()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Остановить связь с МИП");
    connect(pb,SIGNAL(clicked()),this,SLOT(StopMip()));
    hlyout->addWidget(pb);
    vlyout->addLayout(hlyout);
#endif
    cp3->setLayout(vlyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);

#if PROGSIZE != PROGSIZE_EMUL
     if ((!(ModuleBSI::GetHealth() & HTH_REGPARS)) && !StdFunc::IsInEmulateMode()) // есть настроечные коэффициенты в памяти модуля
        ReadTuneCoefs(); // считать их из модуля и показать на экране
#endif
}

QHBoxLayout *TuneDialog85::MipPars(int parnum, const QString &groupname)
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QLabel *lbl = new QLabel(QString::number(parnum) + "." + groupname + ": ");
    hlyout->addWidget(lbl);
    for (int i = 0; i < 3; i++)
    {
        QLabel *lbl = new QLabel(QString::number(i+10,36).toUpper());
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("mip"+QString::number(i+parnum));
        lbl->setToolTip("Параметр "+QString::number(i+parnum));
        lbl->setStyleSheet(ValuesFormat);
        hlyout->addWidget(lbl,10);
    }
    return hlyout;
}


void TuneDialog85::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword; // Ввод пароля
    int (EAbstractTuneDialog::*func)() = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog85::SaveWorkConfig); // Сохранение текущей конфигурации
    /*pf[lbls.at(count++)] = func; // 2. Отображение схемы подключения
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::ShowControlChooseDialog); // Отображение диалога выбора режима контроля показаний
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Show3PhaseScheme); // Отображение схемы подключения
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_2_3); // Проверка связи РЕТОМ и МИП
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_1); // Получение настроечных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_1_1); // Установка настроечных коэффициентов по умолчанию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::SetNewTuneCoefs); // Установка коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_2); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::SaveUeff); // Сохранение значений фильтра
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_3); // Расчёт коррекции по фазе
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_4); // Расчёт коррекции по частоте
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_5); // Отображение ввода трёхфазных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_2); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_6_2); // Расчёт коррекции взаимного влияния каналов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_1); // Получение текущих аналоговых данных и расчёт настроечных коэффициентов по напряжениям
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_2); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_3); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_4); // Ввод измеренных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_5); // Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_6); // Сохранение конфигурации
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_7); // Отображение ввода трёхфазных значений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_8); // Получение текущих аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_7_10); // Расчёт настроечных коэффициентов по токам, напряжениям и углам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_8_1); // Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_8_2); // Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialog80::Start7_3_9); // Восстановление сохранённой конфигурации и проверка
    pf[lbls.at(count++)] = func;*/
}

#if PROGSIZE != PROGSIZE_EMUL
int TuneDialog85::Start7_2_3()
{
    if (TuneControlType == TUNEMIP)
    {
        GED_Type = TD_GED_U; // любой параметр для проверки связи сгодится
        return GetExternalData();
    }
    return Error::ER_NOERROR;
}

int TuneDialog85::Start7_3_1()
{
    if (ModuleBSI::GetHealth() & HTH_REGPARS) // если нет настроечных параметров в памяти модуля
        return Error::ER_RESEMPTY;
    else
    {
        // получение настроечных коэффициентов от модуля
        if (Commands::GetBac(BT_NONE, &Bac_block, sizeof(Bac_block)) != Error::ER_NOERROR)
        {
            EMessageBox::information(this, "Внимание", "Ошибка при приёме данных");
            return Error::ER_GENERALERROR;
        }
        // обновление коэффициентов в соответствующих полях на экране
        FillBac();
        // проверка коэффициентов на правильность в соотв. с п. 7.3.1 "Д2"
        if (CheckTuneCoefs())
            return Error::ER_NOERROR;
        else
            return Error::ER_GENERALERROR;
    }
}

int TuneDialog85::Start7_3_1_1()
{
    if (ModuleBSI::GetHealth() & HTH_REGPARS) // если нет настроечных параметров в памяти модуля
        return Error::ER_RESEMPTY;
    else
    {
        // запись настроечных коэффициентов в модуль
        SetDefCoefs();
        if (Commands::WriteBac(BT_NONE, &Bac_block, sizeof(Bac)) == Error::ER_NOERROR)
        {
            // получение настроечных коэффициентов от модуля
            if (Commands::GetBac(BT_NONE, &Bac_block, sizeof(Bac)) != Error::ER_NOERROR)
            {
                WARNMSG("Ошибка при приёме данных");
                return false;
            }
            // обновление коэффициентов в соответствующих полях на экране
            FillBac();
            return Error::ER_NOERROR;
        }
        else
            return Error::ER_GENERALERROR;
    }
}

int TuneDialog85::Start7_3_2()
{
    ReadAnalogMeasurements();
    return StartCheckAnalogValues(V60, I1, S0, false);
}
#endif

void TuneDialog85::FillBac()
{
    for (int i = 0; i < 3; i++)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i), QString::number(Bac_block.KmU[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+3), QString::number(Bac_block.KmI_1[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+6), QString::number(Bac_block.KmI_4[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+9), QString::number(Bac_block.KmU2[i], 'f', 5));
    }
    WDFunc::SetLEData(this, "tune13", QString::number(Bac_block.K_freq, 'f', 5));

}

void TuneDialog85::FillBackBac()
{
    QString tmps;
    for (int i = 0; i < 3; i++)
    {
        WDFunc::LEData(this, "tune"+QString::number(i), tmps);
        Bac_block.KmU[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+3), tmps);
        Bac_block.KmI_4[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+6), tmps);
        Bac_block.KmI_1[i]=ToFloat(tmps);
        WDFunc::LEData(this, "tune"+QString::number(i+9), tmps);
        Bac_block.KmU2[i]=ToFloat(tmps);
    }
    WDFunc::LEData(this, "tune13", tmps);
    Bac_block.K_freq=ToFloat(tmps);
}

void TuneDialog85::SetDefCoefs()
{
    Bac_block.K_freq = 0.99999999999999999;
    for (int i=0; i<3; i++)
    {
        Bac_block.KmI_1[i] = 1.0;
        Bac_block.KmI_4[i] = 1.0;
    }
    Bac_block.KmU[0] = 0.978;
    Bac_block.KmU[1] = 0.977;
    Bac_block.KmU[2] = 0.977;

    Bac_block.KmU2[0] = 0.97507;
    Bac_block.KmU2[1] = 0.97507;
    Bac_block.KmU2[2] = 0.97507;



    FillBac();
}

void TuneDialog85::CancelTune()
{
    Cancelled = true;
}

void TuneDialog85::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}

float TuneDialog85::ToFloat(QString text)
{
    bool ok;
    float tmpf;
    tmpf = text.toFloat(&ok);
    if (!ok)
    {
        ERMSG("Значение "+text+" не может быть переведено во float");
        return 0;
    }
    return tmpf;
}

int TuneDialog85::SaveWorkConfig()
{
    if (Commands::GetFile(CM_CONFIGFILE,S2Config) == Error::ER_NOERROR)
        memcpy(&Bci_block_work,&C85->Bci_block,sizeof(Config85::Bci));
    else
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}
