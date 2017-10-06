#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGridLayout>
#include <QVBoxLayout>
#include "../widgets/messagebox.h"
#include "../widgets/s_tqcombobox.h"
#include "../widgets/wd_func.h"
#include "../widgets/mystackedwidget.h"
#include "confdialog85.h"

ConfDialog85::ConfDialog85(QVector<publicclass::DataRec> &S2Config, QWidget *parent) :
    AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    this->S2Config = &S2Config;
    C85 = new Config85(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);

    SetupUI();
    PrereadConf();
}

ConfDialog85::~ConfDialog85()
{
}

void ConfDialog85::Fill()
{
/*    int i;
    GeneralConf->Fill(); // заполнение общих полей
    SetComboBox("eq_typecb", QString::number(econf->Bci_block.eq_type));
    SetCheckBox("oscchb.0", (econf->Bci_block.ddosc & 0x0001));
    SetCheckBox("oscchb.1", (econf->Bci_block.ddosc & 0x0002));
    SetCheckBox("oscchb.2", (econf->Bci_block.ddosc & 0x0004));
    SetSpinBox("npointspb", econf->Bci_block.npoints);
    SetSpinBox("nhfiltrspb", econf->Bci_block.nhfiltr);
    switch (pc.ModuleBsi.MTypeM)
    {
    case MTM_81: // 2 напряжения, 0 токов
    {
        SetComboBox("unom.1", QString::number(econf->Bci_block.unom1));
        SetComboBox("unom.2", QString::number(econf->Bci_block.unom2));
        SetSpinBox("thr.1", econf->Bci_block.duosc);
       break;
    }
    case MTM_82:
    {
        SetComboBox("unom.1", QString::number(econf->Bci_block.unom1));
        for (i = 3; i < 6; i++)
            SetSpinBox("inom.2."+QString::number(i), econf->Bci_block.inom2[i]);
        SetSpinBox("thr.1", econf->Bci_block.duosc);
        SetSpinBox("thr.2", econf->Bci_block.diosc);
        break;
    }
    case MTM_83:
    {
        for (i = 0; i < 6; i++)
        {
            SetSpinBox("inom.1."+QString::number(i), econf->Bci_block.inom1[i]);
            SetSpinBox("inom.2."+QString::number(i), econf->Bci_block.inom2[i]);
        }
        SetSpinBox("thr.2", econf->Bci_block.diosc);
       break;
    }
    default:
        break;
    }
    SetSpinBox("thr.3", econf->Bci_block.duimin);*/
}

void ConfDialog85::FillBack()
{

}

void ConfDialog85::SetupUI()
{
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QWidget *cp4 = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFGCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);
    cp4->setStyleSheet(tmps);

    QString paramcolor = MAINWINCLR;
    glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутируемого оборудования:"), 0,0,1,1);
    QStringList cbl = QStringList() << "0. Свободное конфигурирование" << "1. Конденсаторная батарея (фильтр)" \
                                    << "2. Реактор шунтирующий" << "3. Силовой трансформатор" \
                                    << "4. Линия электропередачи";
    s_tqComboBox *cb = WDFunc::NewCB(this, "eq_typecb", cbl, paramcolor);
    cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetEqType(int)));
    glyout->addWidget(cb,0,1,1,1);
    cbl = QStringList() << "Несинхронное" << "Синхронное";
    glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутаций:"), 1,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "включение:"), 0);
    hlyout->addWidget(WDFunc::NewCB(this, "cbvkl", cbl, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "отключение:"), 0);
    hlyout->addWidget(WDFunc::NewCB(this, "cbotkl", cbl, paramcolor), 1);
    glyout->addLayout(hlyout,1,1,1,1);
    hlyout = new QHBoxLayout;
    tmps = "QRadioButton {background-color: "+QString(paramcolor)+";}";
    glyout->addWidget(WDFunc::NewLBL(this, "Наличие напряжения на стороне нагрузки"),2,0,1,1);
    QButtonGroup *bg = new QButtonGroup;
    QRadioButton *rb = new QRadioButton("Есть");
    rb->setObjectName("u2yes");
//    rb->setStyleSheet(tmps);
    bg->addButton(rb);
    hlyout->addWidget(rb);
    rb = new QRadioButton("Нет");
    rb->setObjectName("u2no");
//    rb->setStyleSheet(tmps);
    bg->addButton(rb);
    hlyout->addWidget(rb);
    glyout->addLayout(hlyout, 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Класс напряжения сети, кВ:"), 3,0,1,1);
    cbl = QStringList() << "750" << "550" << "330" << "220" << "110" << "35" << "10";
    glyout->addWidget(WDFunc::NewCB(this, "unomcb", cbl, paramcolor),3,1,1,1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный ток, А:"),4,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "первичный:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "inom1", 10, 50000, 10, 0, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "вторичный:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "inom2", 10, 50000, 10, 0, paramcolor), 1);
    glyout->addLayout(hlyout,4,1,1,1);
    vlyout1->addLayout(glyout);
    MyStackedWidget *stw = new MyStackedWidget;
    stw->setObjectName("eqtypestw");
    // заполняем stackwidget
    QWidget *w = new QWidget;
    QGridLayout *iglyout = new QGridLayout;
    QHBoxLayout *ihlyout = new QHBoxLayout;
    iglyout->addWidget(WDFunc::NewLBL(this, "Задержка отключения относительно перехода через ноль (А,В,С), мс:"),0,0,1,1);
    ihlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    ihlyout->addWidget(WDFunc::NewSPB(this, "tzadoff1", 1, 500, 0.1, 1, paramcolor), 1);
    ihlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    ihlyout->addWidget(WDFunc::NewSPB(this, "tzadoff2", 1, 500, 0.1, 1, paramcolor), 1);
    ihlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    ihlyout->addWidget(WDFunc::NewSPB(this, "tzadoff3", 1, 500, 0.1, 1, paramcolor), 1);
    iglyout->addLayout(ihlyout, 0, 1, 1, 1);
    ihlyout = new QHBoxLayout;
    iglyout->addWidget(WDFunc::NewLBL(this, "Задержка включения относительно перехода через ноль (А,В,С), мс:"),1,0,1,1);
    ihlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    ihlyout->addWidget(WDFunc::NewSPB(this, "tzadon1", 1, 500, 0.1, 1, paramcolor), 1);
    ihlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    ihlyout->addWidget(WDFunc::NewSPB(this, "tzadon2", 1, 500, 0.1, 1, paramcolor), 1);
    ihlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    ihlyout->addWidget(WDFunc::NewSPB(this, "tzadon3", 1, 500, 0.1, 1, paramcolor), 1);
    iglyout->addLayout(ihlyout, 1, 1, 1, 1);
    w->setLayout(iglyout);
    stw->insertWidget(0, w); // eq_type = 0 => свободно конфигурируемый режим
    w = new QWidget;
    stw->insertWidget(1, w); // eq_type = 1 => конденсаторная батарея, пока пусто
    w = new QWidget;
    stw->insertWidget(2, w); // eq_type = 2 => реактор шунтирующий, пока пусто
    w = new QWidget;
    iglyout = new QGridLayout;
    iglyout->addWidget(WDFunc::NewLBL(this, "Тип магнитопровода:"),0,0,1,1);
    cbl = QStringList() << "Три однофазных" << "Трехфазный трехстержневой" << "Трехфазный пятистержневой (с шунтом)";
    iglyout->addWidget(WDFunc::NewCB(this, "core_type", cbl, paramcolor),0,1,1,1);
    iglyout->addWidget(WDFunc::NewLBL(this, "Наличие обмотки, соединенной в треугольник:"),1,0,1,1);
    cbl = QStringList() << "Нет" << "Есть";
    iglyout->addWidget(WDFunc::NewCB(this, "d_win", cbl, paramcolor),1,1,1,1);
    iglyout->addWidget(WDFunc::NewLBL(this, "Схема подключения нейтрали:"),2,0,1,1);
    cbl = QStringList() << "Заземленная" << "Изолированная";
    iglyout->addWidget(WDFunc::NewCB(this, "n_type", cbl, paramcolor),2,1,1,1);
    iglyout->addWidget(WDFunc::NewLBL(this, "Учет остаточного магнитного потока при коммутации:"),3,0,1,1);
    cbl = QStringList() << "Нет" << "Да";
    iglyout->addWidget(WDFunc::NewCB(this, "residue", cbl, paramcolor),3,1,1,1);
    w->setLayout(iglyout);
    stw->insertWidget(3, w); // eq_type = 3 => силовой трансформатор
    w = new QWidget;
    stw->insertWidget(4, w); // eq_type = 4 => линия электропередачи, пока пусто
    vlyout1->addWidget(stw, 1);
    vlyout1->addStretch(1);
    cp1->setLayout(vlyout1);

    vlyout1 = new QVBoxLayout;
    glyout = new QGridLayout;
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный ток, А:"),0,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "inomspb", 10, 50000, 10, 0,paramcolor),0,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Максимальный коммутируемый ток, кА:"),1,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "ikzspb", 1, 150, 0.1, 1,paramcolor),1,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Минимальное время горения дуги, мс:"),2,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "tarcspb", 10, 50000, 10, 0,paramcolor),2,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Собственное время отключения (А,В,С), мс:"),3,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tsoff1", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tsoff2", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tsoff3", 1, 500, 0.1, 1, paramcolor), 1);
    glyout->addLayout(hlyout, 3, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Собственное время включения (А,В,С), мс:"),4,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tson1", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tson2", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tson3", 1, 500, 0.1, 1, paramcolor), 1);
    glyout->addLayout(hlyout, 4, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Мех. задержка сраб. б/к откл. состояния (А,В,С), мс:"),5,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkoff1", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkoff2", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkoff3", 1, 500, 0.1, 1, paramcolor), 1);
    glyout->addLayout(hlyout, 5, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Мех. задержка сраб. б/к вкл. состояния (А,В,С), мс:"),6,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkon1", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkon2", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkon3", 1, 500, 0.1, 1, paramcolor), 1);
    glyout->addLayout(hlyout, 6, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Скорость снижения проб. напряжения при вкл., мс:"),7,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "rddsspb", 10, 50000, 10, 0,paramcolor),7,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Скорость роста проб. напряжения при откл., мс:"),8,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "rrdsspb", 10, 50000, 10, 0,paramcolor),8,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Допустимое время включения соленоида вкл., мс:"),9,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "tsolonspb", 10, 50000, 10, 0,paramcolor),9,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Допустимое время включения соленоида откл., мс:"),10,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "tsoloffspb", 10, 50000, 10, 0,paramcolor),10,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Количество операций отключения (А,В,С):"),11,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomoff1", 0, 1000000, 1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomoff2", 0, 1000000, 1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomoff3", 0, 1000000, 1, 1, paramcolor), 1);
    glyout->addLayout(hlyout, 11, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Количество операций включения (А,В,С):"),12,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomon1", 0, 1000000, 1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomon2", 0, 1000000, 1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomon3", 0, 1000000, 1, 1, paramcolor), 1);
    glyout->addLayout(hlyout, 12, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальный электрический износ (А,В,С), кА²:"),13,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "reskz1", 0, 100000, 1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "reskz2", 0, 100000, 1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "reskz3", 0, 100000, 1, 1, paramcolor), 1);
    glyout->addLayout(hlyout, 13, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Предельное значение мех. ресурса:"),14,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "resnommaxspb", 1000, 1000000, 1, 0,paramcolor),14,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Предельное значение комм. ресурса, кА²:"),15,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "reskzmaxspb", 100, 50000, 1, 0,paramcolor),15,1,1,1);
    vlyout1->addLayout(glyout);
    vlyout1->addStretch(1);
    cp2->setLayout(vlyout1);

    vlyout1 = new QVBoxLayout;
    glyout = new QGridLayout;
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Использовать адаптивный алгоритм:"),0,0,1,1);
    QCheckBox *chb = new QCheckBox;
    chb->setObjectName("adapt");
    connect(chb,SIGNAL(clicked(bool)),this,SLOT(ShowAdaptParams(bool)));
    glyout->addWidget(chb, 0, 1, 1, 1);
    QGridLayout *glyout2 = new QGridLayout;
    QGroupBox *gb = new QGroupBox("AUX");
    QStringList fl = QStringList() << "Pc" << "Pb" << "Pa" << "Tc" << "Tb" << "Ta" << "To" << "Us";
    QStringList tl = QStringList() << "Давление в гидроприводе ф. С" << "Давление в гидроприводе ф. B" << "Давление в гидроприводе ф. A" << \
                                      "Температура в приводе ф. С" << "Температура в приводе ф. B" << "Температура в приводе ф. A" << \
                                      "Температура окружающей среды" << "Напряжение питания соленоидов";
    for (int i=0; i<fl.size(); ++i)
    {
        glyout2->addWidget(WDFunc::NewLBLT(this, fl.at(i), "", "", tl.at(i)),0,i,1,1);
        glyout2->addWidget(WDFunc::NewChB(this, fl.at(i), ""),1,i,1,1);
    }
    gb->setLayout(glyout2);
    glyout->addWidget(gb, 1, 0, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка скачка напряжения для запуска осциллографирования, %:"),2,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "duosc", 0, 99, 0.1, 1, paramcolor),2,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка скачка тока для запуска осциллографирования, %:"),3,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "diosc", 0, 99, 0.1, 1, paramcolor),3,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума напряжения, %:"),4,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dumin", 0, 99, 0.1, 1, paramcolor),4,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума тока, %:"),5,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dimin", 0, 99, 0.1, 1, paramcolor),5,1,1,1);
    vlyout1->addLayout(glyout);
    stw = new MyStackedWidget;
    stw->setObjectName("adaptstw");
    w = new QWidget;
    stw->insertWidget(0, w); // неадаптивный - пусто
    w = new QWidget;
    iglyout = new QGridLayout;
    iglyout->addWidget(WDFunc::NewLBL(this, "Коэф. коррекции времени включения по U соленоида:"),0,0,1,1);
    iglyout->addWidget(WDFunc::NewSPB(this, "k_on_volt", -1000, 1000, 0.1, 1, paramcolor),0,1,1,1);
    iglyout->addWidget(WDFunc::NewLBL(this, "Коэф. коррекции времени отключения по U соленоида:"),1,0,1,1);
    iglyout->addWidget(WDFunc::NewSPB(this, "k_off_volt", -1000, 1000, 0.1, 1, paramcolor),1,1,1,1);
    iglyout->addWidget(WDFunc::NewLBL(this, "Коэф. коррекции времени включения по T окр. среды:"),2,0,1,1);
    iglyout->addWidget(WDFunc::NewSPB(this, "k_on_tamb", -1000, 1000, 0.1, 1, paramcolor),2,1,1,1);
    iglyout->addWidget(WDFunc::NewLBL(this, "Коэф. коррекции времени отключения по T окр. среды:"),3,0,1,1);
    iglyout->addWidget(WDFunc::NewSPB(this, "k_off_tamb", -1000, 1000, 0.1, 1, paramcolor),3,1,1,1);
    iglyout->addWidget(WDFunc::NewLBL(this, "Коэф. коррекции времени включения по T в приводе:"),4,0,1,1);
    iglyout->addWidget(WDFunc::NewSPB(this, "k_on_tdr", -1000, 1000, 0.1, 1, paramcolor),4,1,1,1);
    iglyout->addWidget(WDFunc::NewLBL(this, "Коэф. коррекции времени отключения по T в приводе:"),5,0,1,1);
    iglyout->addWidget(WDFunc::NewSPB(this, "k_off_tdr", -1000, 1000, 0.1, 1, paramcolor),5,1,1,1);
    iglyout->addWidget(WDFunc::NewLBL(this, "Коэф. коррекции времени включения по P в приводе:"),6,0,1,1);
    iglyout->addWidget(WDFunc::NewSPB(this, "k_on_hdr", -1000, 1000, 0.1, 1, paramcolor),6,1,1,1);
    iglyout->addWidget(WDFunc::NewLBL(this, "Коэф. коррекции времени отключения по P в приводе:"),7,0,1,1);
    iglyout->addWidget(WDFunc::NewSPB(this, "k_off_hdr", -1000, 1000, 0.1, 1, paramcolor),7,1,1,1);
    w->setLayout(iglyout);
    stw->insertWidget(1, w); // адаптивный - доп. параметры
    vlyout1->addWidget(stw, 1);
    vlyout1->addStretch(1);
    cp3->setLayout(vlyout1);

    vlyout1 = new QVBoxLayout;
    glyout = new QGridLayout;
    QStringList lbls = QStringList() << "Собственное время отключения, мс:" << \
                                        "Собственное время включения, мс:" << \
                                        "Время перемещения главного контакта при отключении, мс:" << \
                                        "Время перемещения главного контакта при включении, мс:" << \
                                        "Погрешность выполненной синхронной коммутации при отключении, мс:" << \
                                        "Погрешность выполненной синхронной коммутации при включении, мс:" << \
                                        "Время рассинхронизации фаз выключателя, мс:" << \
                                        "Время горения дуги в полюсе выключателя, мс:" << \
                                        "Остаточный механический ресурс, ед:" << \
                                        "Остаточный коммутационный ресурс, ед:" << \
                                        "Ток в выключателе во время коммутации, А";
    QStringList names = QStringList() << "ts_off" << "ts_on" << "tmain_off" << "tmain_on" << \
                                     "dt_off" << "dt_on" << "tras" << "tarc" << "resnom" << \
                                     "reskz" << "i";
    for (int i=0; i<lbls.size(); ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(this, lbls.at(i)),i,0,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, names.at(i)+"pred", 0, 1000, 0.01, 2, ACONFYCLR),i,1,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, names.at(i)+"alarm", 0, 1000, 0.01, 2, ACONFRCLR),i,2,1,1);
    }
    vlyout1->addLayout(glyout);
    vlyout1->addStretch(1);
    cp4->setLayout(vlyout1);

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(cp1,"Основные");
    ConfTW->addTab(cp2,"Параметры выключателя");
    ConfTW->addTab(cp3,"Доп. параметры по алгоритму");
    ConfTW->addTab(cp4,"Уставки сигнализации");
    lyout->addWidget(ConfTW);
    setLayout(lyout);
}

void ConfDialog85::CheckConf()
{
}

QWidget *ConfDialog85::UNom(int numunom)
{
    QWidget *w = new QWidget;
    QString NumUNomStr = QString::number(numunom);
    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QLabel *lbl=new QLabel("Класс напряжения "+NumUNomStr+"-й группы, кВ:");
    gb2lyout->addWidget(lbl);
    QStringList cbl = QStringList() << "1150" << "750" << "500" << "330" << "220" << "110" << "35" << "21" << "15.75" << "11" << "10" << "6.3";
    s_tqComboBox *cb = WDFunc::NewCB(this, "unom."+NumUNomStr, cbl, ACONFWCLR);
    cb->setEditable(true);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetVoltageClass(int)));
    gb2lyout->addWidget(cb);
    w->setLayout(gb2lyout);
    return w;
}

// 1 - первичный ток первой группы, 2 - вторичный ток первой группы, 3,4 - то же по второй группе

QWidget *ConfDialog85::INom(int numinom)
{
    QWidget *w = new QWidget;
/*    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QString NumGroup = (numinom < 3) ? "1" : "2";
    QString Perv = (numinom%2) ? "первичные" : "вторичные";
    QString PervNum = (numinom%2) ? "1" : "2";
    QLabel *lbl=new QLabel("Номинальные "+Perv+" токи ТТ "+NumGroup+"-й группы, А: ");
    gb2lyout->addWidget(lbl, 0);
    for (int i = 0; i < 3; i++)
    {
        lbl=new QLabel(QString::number(i+10, 16).toUpper() + ":"); // A, B, C
        gb2lyout->addWidget(lbl, 1, Qt::AlignRight);
        // inom.<группа 1|2>.<тип перв/втор 1|2>.<phase>
        QDoubleSpinBox *dspbls = WDFunc::NewSPB(this, "inom."+NumGroup+"."+PervNum+"."+QString::number(i), 1, 50000, 1, 0, ACONFWCLR);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(SetCurrent(double)));
        gb2lyout->addWidget(dspbls, 1);
    }
    w->setLayout(gb2lyout);*/
    return w;
}

QWidget *ConfDialog85::Threshold(QString str, int numthr)
{
    QWidget *w = new QWidget;
/*    QHBoxLayout *gb2lyout = new QHBoxLayout;
    QLabel *lbl = new QLabel(str);
    gb2lyout->addWidget(lbl);
    QDoubleSpinBox *dspbls = WDFunc::NewSPB(this, "thr."+QString::number(numthr), 0, 100, 0.1, 1, ACONFWCLR);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(SetThreshold(double)));
    gb2lyout->addWidget(dspbls);
    w->setLayout(gb2lyout);*/
    return w;
}

void ConfDialog85::SetOsc(int isChecked)
{
/*    int val;
    if ((val = GetChNumFromObjectName(sender()->objectName()) == GENERALERROR))
    {
        DBGMSG;
        return;
    }
    quint16 tmpint = 0x0001;
    tmpint = tmpint << val;
    if (isChecked == Qt::Checked)
        C85->Bci_block.ddosc |= tmpint;
    else
        C85->Bci_block.ddosc &= ~tmpint;*/
}

void ConfDialog85::SetThreshold(double dbl)
{
/*    int thr;
    if ((thr = GetChNumFromObjectName(sender()->objectName()) == GENERALERROR))
    {
        DBGMSG;
        return;
    }
    switch (thr)
    {
    case 1: // % напряжения
    {
        C85->Bci_block.duosc = dbl;
        break;
    }
    case 2: // % тока
    {
        C85->Bci_block.diosc = dbl;
        break;
    }
    case 3: // % частоты
    {
        C85->Bci_block.duimin = dbl;
        break;
    }
    default:
        break;
    }*/
}

void ConfDialog85::SetVoltageClass(int tmpi)
{
/*    int vnum;
    if ((vnum = GetChNumFromObjectName(sender()->objectName()) == GENERALERROR))
    {
        DBGMSG;
        return;
    }
    if (vnum == 2) // вторая тройка напряжений
        C85->Bci_block.unom2 = tmpi;
    else
        C85->Bci_block.unom1 = tmpi;*/
}

void ConfDialog85::SetCurrent(double dbl)
{
/*   QStringList sl = sender()->objectName().split(".");
    if (sl.size() < 4) // name, group, perv, phase
    {
        DBGMSG;
        return;
    }
    bool ok;
    int group, perv, phase;
    group = sl.at(1).toInt(&ok);
    if (ok)
    {
        perv = sl.at(2).toInt(&ok);
        if (ok)
        {
            phase = sl.at(3).toInt(&ok);
            if (ok)
            {
                if (perv == 1) // первичная группа
                    C85->Bci_block.inom1[(group-1)*3+phase] = dbl;
                else
                    C85->Bci_block.inom2[(group-1)*3+phase] = dbl;
                return;
            }
        }
    }
    DBGMSG;*/
}

void ConfDialog85::SetEqType(int tmpi)
{
    MyStackedWidget *stw = this->findChild<MyStackedWidget *>("eqtypestw");
    if (stw != 0)
    {
        if (tmpi < stw->count())
            stw->setCurrentIndex(tmpi);
    }
}

void ConfDialog85::SetNPoints(QString tmpi)
{
//    C85->Bci_block.npoints = tmpi.toInt();
}

void ConfDialog85::SetNFiltr(int tmpi)
{
//    C85->Bci_block.nfiltr = tmpi;
}

void ConfDialog85::SetNHFiltr(int tmpi)
{
//    C85->Bci_block.nhfiltr = tmpi;
}

void ConfDialog85::SetDefConf()
{
    //    C85->SetDefConf();
}

void ConfDialog85::ShowAdaptParams(bool isAdaptChecked)
{
    MyStackedWidget *stw = this->findChild<MyStackedWidget *>("adaptstw");
    if (stw != 0)
    {
        if (isAdaptChecked)
            stw->setCurrentIndex(1);
        else
            stw->setCurrentIndex(0);
    }
}
