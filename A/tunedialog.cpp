#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include "tunedialog.h"
#include "config.h"

a_tunedialog::a_tunedialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    cn = new canal;
    for (int i = 0; i < 16; i++)
    {
        Bda0.sin[i] = 0;
        Bda5.sin[i] = UINT_MAX;
        Bda20.sin[i] = UINT_MAX;
    }
    SetupUI();
}

void a_tunedialog::SetupUI()
{
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: blue; font: bold 10px;}";
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl;
    QVBoxLayout *gb1lyout = new QVBoxLayout;
    QHBoxLayout *gb2lyout = new QHBoxLayout;

    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->addTab(cp1,"Коэффициенты");
    TuneTW->addTab(cp2,"Настройка");
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel("b"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("tunebcoef"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 1);
        lbl = new QLabel("u"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("tunek1coef"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 1);
        lbl = new QLabel("i"+QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("tunek2coef"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 1);
        if ((i>0)&&!((i+1)%2))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);
    QPushButton *pb = new QPushButton("Прочитать настроечные коэффициенты из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
    gb1lyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в модуль");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefs()));
    gb1lyout->addWidget(pb);
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);
    cp1->setLayout(lyout);

    lyout = new QVBoxLayout;
    gb = new QGroupBox("Настройка нуля");
    gb->setStyleSheet("QGroupBox {background-color: #99FFCC;}");
    gb1lyout = new QVBoxLayout;
    gb2lyout = new QHBoxLayout;
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel(QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("tune0ch"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 5);
        if ((i>1)&&!((i+1)%6))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);
    pb = new QPushButton("Запустить настройку нуля");
    pb->setStyleSheet("QPushButton {background-color: #99FFCC;}");
    connect(pb,SIGNAL(clicked()),this,SLOT(tune0()));
    gb1lyout->addWidget(pb);
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Настройка 5 В");
    gb->setStyleSheet("QGroupBox {background-color: #FFCC99;}");
    gb1lyout = new QVBoxLayout;
    gb2lyout = new QHBoxLayout;
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel(QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("tune5ch"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 5);
        if ((i>1)&&!((i+1)%6))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);
    pb = new QPushButton("Запустить настройку 5 В");
    pb->setStyleSheet("QPushButton {background-color: #FFCC99;}");
    connect(pb,SIGNAL(clicked()),this,SLOT(tune5()));
    gb1lyout->addWidget(pb);
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);

    gb = new QGroupBox("Настройка 20 мА");
    gb->setStyleSheet("QGroupBox {background-color: #CC99FF;}");
    gb1lyout = new QVBoxLayout;
    gb2lyout = new QHBoxLayout;
    for (i = 0; i < 16; i++)
    {
        lbl = new QLabel(QString::number(i)+":");
        gb2lyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setObjectName("tune20ch"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        gb2lyout->addWidget(lbl, 5);
        if ((i>1)&&!((i+1)%6))
        {
            gb1lyout->addLayout(gb2lyout);
            gb2lyout = new QHBoxLayout;
        }
    }
    if (gb2lyout->count())
        gb1lyout->addLayout(gb2lyout);
    pb = new QPushButton("Запустить настройку 20 мА");
    pb->setStyleSheet("QPushButton {background-color: #CC99FF;}");
    connect(pb,SIGNAL(clicked()),this,SLOT(tune20()));
    gb1lyout->addWidget(pb);
    gb->setLayout(gb1lyout);
    lyout->addWidget(gb);
    cp2->setLayout(lyout);
    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
}

void a_tunedialog::tune(int tunenum)
{
    switch (tunenum)
    {
    case 0:
    {
        if (QMessageBox::information(this,"Настройка",\
                                     "Переключите входные переключатели на ток,\nустановите напряжение 0 В " \
                                     "на всех\nвходах модуля и нажмите OK")\
                == QMessageBox::Ok)
        {
            connect(cn,SIGNAL(DataReady()),this,SLOT(CheckAndShowTune0()));
            cn->Send(Gda, &Bda0, sizeof(Bda));
        }
        break;
    }
    case 5:
    {
        if (QMessageBox::information(this,"Настройка",\
                                     "Переключите входные переключатели на напряжение,\nустановите напряжение" \
                                     "5 В на всех\nвходах модуля и нажмите OK")\
                == QMessageBox::Ok)
        {
            connect(cn,SIGNAL(DataReady()),this,SLOT(CheckAndShowTune5()));
            cn->Send(Gda, &Bda5, sizeof(Bda));
        }
        break;
    }
    case 20:
    {
        if (QMessageBox::information(this,"Настройка",\
                                     "Переключите входные переключатели на ток,\nустановите ток 20 мА на всех" \
                                     "\nвходах модуля и нажмите OK") == QMessageBox::Ok)
        {
            connect(cn,SIGNAL(DataReady()),this,SLOT(CheckAndShowTune20()));
            cn->Send(Gda, &Bda20, sizeof(Bda));
        }
        break;
    }
    default:
        break;
    }
}

void a_tunedialog::tune0()
{
    tune(0);
}

void a_tunedialog::CheckAndShowTune0()
{
    disconnect(cn,SIGNAL(DataReady()),this,SLOT(CheckAndShowTune0()));
    if (cn->result)
    {
        ShowErrMsg(cn->result);
        return;
    }
    for (int i = 0; i < 16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("tune0ch"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda0.sin[i]/Z,16));
    }
    CalcNewTuneCoefs();
    RefreshTuneCoefs();
}

void a_tunedialog::tune5()
{
    tune(5);
}

void a_tunedialog::CheckAndShowTune5()
{
    disconnect(cn,SIGNAL(DataReady()),this,SLOT(CheckAndShowTune5()));
    if (cn->result)
    {
        ShowErrMsg(cn->result);
        return;
    }
    for (int i = 0; i < 16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("tune5ch"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda5.sin[i]/Z,16));
    }
    CalcNewTuneCoefs();
    RefreshTuneCoefs();
}

void a_tunedialog::tune20()
{
    tune(20);
}

void a_tunedialog::CheckAndShowTune20()
{
    disconnect(cn,SIGNAL(DataReady()),this,SLOT(CheckAndShowTune20()));
    if (cn->result)
    {
        ShowErrMsg(cn->result);
        return;
    }
    for (int i = 0; i < 16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("tune20ch"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda20.sin[i]/Z,16));
    }
    CalcNewTuneCoefs();
    RefreshTuneCoefs();
}

void a_tunedialog::ReadTuneCoefs()
{

}

void a_tunedialog::WriteTuneCoefs()
{

}

void a_tunedialog::CalcNewTuneCoefs()
{
    for (int i = 0; i < 16; i++)
    {
        Bac_block[i].fbin = 1.25 - (static_cast<float>(Bda0.sin[i])/(Z*4095.0)*2.5);
        if ((Bda0.sin[i] == Bda5.sin[i]) || (Bda0.sin[i] == Bda20.sin[i]))
        {
            QMessageBox::warning(this,"Предупреждение","Ошибки в настроечных коэффициентах\nДеление на ноль");
            return;
        }
        Bac_block[i].fkuin = 1 / (2.5/(Z*4095.0)*static_cast<float>(Bda0.sin[i]-Bda5.sin[i]));
        Bac_block[i].fkiin = 1 / (2.5/(Z*4095.0)*static_cast<float>(Bda0.sin[i]-Bda20.sin[i]));
    }
}

void a_tunedialog::RefreshTuneCoefs()
{
    for (int i = 0; i < 16; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("tunebcoef"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bac_block[i].fbin, 'f', 5));
        lbl = this->findChild<QLabel *>("tunek1coef"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString("%1").arg(Bac_block[i].fkuin, 0, 'f', 5));
        lbl = this->findChild<QLabel *>("tunek2coef"+QString::number(i));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bac_block[i].fkiin, 'f', 5));
    }
}

void a_tunedialog::ShowErrMsg(int ermsg)
{
    QMessageBox::critical(this,"error!",errmsgs.at(ermsg));
}
