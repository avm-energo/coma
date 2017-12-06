#include <QtMath>
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
    int cbidx = (C85->Bci_block.TypeA & 0x04) ? 2 : ((C85->Bci_block.TypeA & 0x02) ? 1 : 0);
    WDFunc::SetCBIndex(this, "typea", cbidx);
    WDFunc::SetSPBData(this, "numa", C85->Bci_block.NumA);
    WDFunc::SetCBIndex(this, "eq_type", C85->Bci_block.Eq_type);
    WDFunc::SetCBIndex(this, "optype", C85->Bci_block.Op_type);
    WDFunc::SetRBData(this, "u2yes", (C85->Bci_block.Is_U2 == 1));
    WDFunc::SetRBData(this, "u2no", (C85->Bci_block.Is_U2 == 0));
    WDFunc::SetCBData(this, "unomcb", QString::number(C85->Bci_block.Unom));
    WDFunc::SetSPBData(this, "inom1", C85->Bci_block.ITT1nom);
    WDFunc::SetCBData(this, "inom2", QString::number(C85->Bci_block.ITT2nom));
    for (int i=0; i<3; ++i)
    {
        WDFunc::SetSPBData(this, "tzadoff"+QString::number(i+1), C85->Bci_block.Tzad_OFF[i]);
        WDFunc::SetSPBData(this, "tzadon"+QString::number(i+1), C85->Bci_block.Tzad_ON[i]);
        WDFunc::SetSPBData(this, "tsoff"+QString::number(i+1), C85->Bci_block.Ts_OFF[i]);
        WDFunc::SetSPBData(this, "tson"+QString::number(i+1), C85->Bci_block.Ts_ON[i]);
        WDFunc::SetSPBData(this, "tbkoff"+QString::number(i+1), C85->Bci_block.Tbk_OFF[i]);
        WDFunc::SetSPBData(this, "tbkon"+QString::number(i+1), C85->Bci_block.Tbk_ON[i]);
        WDFunc::SetSPBData(this, "resnomoff"+QString::number(i+1), C85->Bci_block.RESnom_OFF[i]);
        WDFunc::SetSPBData(this, "resnomon"+QString::number(i+1), C85->Bci_block.RESnom_ON[i]);
        WDFunc::SetSPBData(this, "reskz"+QString::number(i+1), C85->Bci_block.RESkz[i]);
    }
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
    int row = 0;
    glyout->addWidget(WDFunc::NewLBL(this, "Тип аппарата:"), row,0,1,1);
    QStringList cbl = QStringList() << "0. Выключатель CB" << "1. Заземлитель G" << "2. Резъединитель D";
    s_tqComboBox *cb = WDFunc::NewCB(this, "typea", cbl, paramcolor);
    cb->setMinimumWidth(70);
    glyout->addWidget(cb,row,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Номер аппарата:"), row,2,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "numa", 1, 999, 1, 0, paramcolor), row,3,1,1);
    ++row;
    glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутируемого оборудования:"), row,0,1,1);
    cbl = QStringList() << "0. Свободное конфигурирование" << "1. Конденсаторная батарея (фильтр)" \
                                    << "2. Реактор шунтирующий" << "3. Силовой трансформатор" \
                                    << "4. Линия электропередачи";
    cb = WDFunc::NewCB(this, "eq_typecb", cbl, paramcolor);
    cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetEqType(int)));
    glyout->addWidget(cb,row++,1,1,1);

    glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутаций:"), row,0,1,1);
    cbl = QStringList() << "Несинхронные включение и отключение" << "Cинхронное включение" << \
                           "Синхронное отключение" << "Синхронное включение и отключение";
    glyout->addWidget(WDFunc::NewCB(this, "optype", cbl, paramcolor),row++,1,1,1);
    hlyout = new QHBoxLayout;
    tmps = "QRadioButton {background-color: "+QString(paramcolor)+";}";
    glyout->addWidget(WDFunc::NewLBL(this, "Наличие напряжения на стороне нагрузки"),row,0,1,1);
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
    glyout->addLayout(hlyout, row++, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Класс напряжения сети, кВ:"), row,0,1,1);
    cbl = QStringList() << "750" << "550" << "330" << "220" << "110" << "35" << "10";
    glyout->addWidget(WDFunc::NewCB(this, "unomcb", cbl, paramcolor),row++,1,1,1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный ток, А:"),row,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "первичный:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "inom1", 10, 50000, 10, 0, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "вторичный:"), 0);
    cbl = QStringList() << "1" << "5";
    hlyout->addWidget(WDFunc::NewCB(this, "inom2", cbl, paramcolor), 1);
    glyout->addLayout(hlyout,row,1,1,1);
    vlyout1->addLayout(glyout);
    MyStackedWidget *stw = new MyStackedWidget;
    stw->setObjectName("eqtypestw");
    // заполняем stackwidget
    QWidget *w = new QWidget;
    QGridLayout *iglyout = new QGridLayout;
    QHBoxLayout *ihlyout = new QHBoxLayout;
    iglyout->addWidget(WDFunc::NewLBL(this, "Задержка отключения относительно перехода\nчерез ноль напряжения (А,В,С), мс:"),0,0,1,1);
    ihlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    ihlyout->addWidget(WDFunc::NewSPB(this, "tzadoff1", 1, 500, 0.1, 1, paramcolor), 1);
    ihlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    ihlyout->addWidget(WDFunc::NewSPB(this, "tzadoff2", 1, 500, 0.1, 1, paramcolor), 1);
    ihlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    ihlyout->addWidget(WDFunc::NewSPB(this, "tzadoff3", 1, 500, 0.1, 1, paramcolor), 1);
    iglyout->addLayout(ihlyout, 0, 1, 1, 1);
    ihlyout = new QHBoxLayout;
    iglyout->addWidget(WDFunc::NewLBL(this, "Задержка включения относительно перехода\nчерез ноль напряжения (А,В,С), мс:"),1,0,1,1);
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
    glyout->addWidget(WDFunc::NewLBL(this, "Собственное время отключения (А,В,С), мс:"),2,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tsoff1", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tsoff2", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tsoff3", 1, 500, 0.1, 1, paramcolor), 1);
    glyout->addLayout(hlyout, 2, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Собственное время включения (А,В,С), мс:"),3,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tson1", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tson2", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tson3", 1, 500, 0.1, 1, paramcolor), 1);
    glyout->addLayout(hlyout, 3, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Мех. задержка сраб. б/к откл. состояния (А,В,С), мс:"),4,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkoff1", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkoff2", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkoff3", 1, 500, 0.1, 1, paramcolor), 1);
    glyout->addLayout(hlyout, 4, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Мех. задержка сраб. б/к вкл. состояния (А,В,С), мс:"),5,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkon1", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkon2", 1, 500, 0.1, 1, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "tbkon3", 1, 500, 0.1, 1, paramcolor), 1);
    glyout->addLayout(hlyout, 5, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Минимальное время горения дуги, мс:"),6,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "tarcspb", 10, 50000, 10, 0,paramcolor),6,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Скорость снижения проб. напряжения при вкл., кВ/мс:"),7,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "rddsspb", 10, 50000, 10, 0,paramcolor),7,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Скорость роста проб. напряжения при откл., кВ/мс:"),8,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "rrdsspb", 10, 50000, 10, 0,paramcolor),8,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Допустимое время включения соленоида вкл., мс:"),9,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "tsolonspb", 10, 50000, 10, 0,paramcolor),9,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Допустимое время включения соленоида откл., мс:"),10,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "tsoloffspb", 10, 50000, 10, 0,paramcolor),10,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное количество операций отключения (А,В,С):"),11,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomoff1", 0, 1000000, 1, 0, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomoff2", 0, 1000000, 1, 0, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomoff3", 0, 1000000, 1, 0, paramcolor), 1);
    glyout->addLayout(hlyout, 11, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное количество операций включения (А,В,С):"),12,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomon1", 0, 1000000, 1, 0, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomon2", 0, 1000000, 1, 0, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "resnomon3", 0, 1000000, 1, 0, paramcolor), 1);
    glyout->addLayout(hlyout, 12, 1, 1, 1);
    hlyout = new QHBoxLayout;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальный электрический износ (А,В,С), кА²:"),13,0,1,1);
    hlyout->addWidget(WDFunc::NewLBL(this, "А:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "reskz1", 0, 100000, 1, 0, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "В:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "reskz2", 0, 100000, 1, 0, paramcolor), 1);
    hlyout->addWidget(WDFunc::NewLBL(this, "С:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "reskz3", 0, 100000, 1, 0, paramcolor), 1);
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
    QGroupBox *gb = new QGroupBox("Использование сигналов в алгоритме");
    QStringList fl = QStringList() << "Pa" << "Pb" << "Pc" << "Ta" << "Tb" << "Tc" << "To" << "Us";
    QStringList tl = QStringList() << "Давление в гидроприводе ф. A" << "Давление в гидроприводе ф. B" << "Давление в гидроприводе ф. C" << \
                                      "Температура в приводе ф. A" << "Температура в приводе ф. B" << "Температура в приводе ф. C" << \
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
                                        "Остаточный механический ресурс, операций:" << \
                                        "Остаточный коммутационный ресурс, кА²:" << \
                                        "Ток в выключателе во время коммутации, А";
    QStringList names = QStringList() << "ts_off" << "ts_on" << "tmain_off" << "tmain_on" << \
                                     "dt_off" << "dt_on" << "tras" << "tarc" << "resnom" << \
                                     "reskz" << "i";
    QList<int> tolerances = QList<int>() << 0 << 0 << 0 << 0 << 0<<0<<0<<0<<0<<0<<2;
    for (int i=0; i<lbls.size(); ++i)
    {
        float step = (tolerances.at(i) == 0) ? 1 : (1 / qPow(10, tolerances.at(i)));
        glyout->addWidget(WDFunc::NewLBL(this, lbls.at(i)),i,0,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, names.at(i)+"pred", 0, 1000, step, tolerances.at(i), ACONFYCLR),i,1,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, names.at(i)+"alarm", 0, 1000, step, tolerances.at(i), ACONFRCLR),i,2,1,1);
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
    CheckConfErrors.clear();
    if (C85->Bci_block.dT_OFFpred > C85->Bci_block.dT_OFFalarm)
        CheckConfErrors.append("dT_OFFpred > dT_OFFalarm");
    if (C85->Bci_block.dT_OFFpred > C85->Bci_block.dT_OFFalarm)
        CheckConfErrors.append("dT_OFFpred > dT_OFFalarm");
    if (C85->Bci_block.dT_OFFpred > C85->Bci_block.dT_OFFalarm)
        CheckConfErrors.append("dT_OFFpred > dT_OFFalarm");
    if (C85->Bci_block.dT_OFFpred > C85->Bci_block.dT_OFFalarm)
        CheckConfErrors.append("dT_OFFpred > dT_OFFalarm");
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

void ConfDialog85::SetDefConf()
{
    C85->SetDefConf();
    FillBack();
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
