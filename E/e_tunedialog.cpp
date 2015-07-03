#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QThread>
#include <QCoreApplication>
#include "e_tunedialog.h"
#include "../publicclass.h"
#include "../canal.h"

e_tunedialog::e_tunedialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void e_tunedialog::SetupUI()
{
    int i;
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: blue; font: bold 10px;}";
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->addTab(cp1,"Коэффициенты");
    TuneTW->addTab(cp2,"Настройка");
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < 6; i++)
    {
        lbl = new QLabel("KmU"+QString::number(i));
        glyout->addWidget(lbl,0,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("kmu"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,1,i,1,1);
        lbl = new QLabel("KmI5"+QString::number(i));
        glyout->addWidget(lbl,2,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("kmi5"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,3,i,1,1);
        lbl = new QLabel("KmI1"+QString::number(i));
        glyout->addWidget(lbl,4,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("kmi1"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,5,i,1,1);
        lbl = new QLabel("DPsi"+QString::number(i));
        glyout->addWidget(lbl,6,i,1,1);
        lbl = new QLabel("");
        lbl->setObjectName("dpsi"+QString::number(i));
        lbl->setStyleSheet(ValuesFormat);
        glyout->addWidget(lbl,7,i,1,1);
    }
    lbl=new QLabel("K_freq:");
    lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,8,0,1,1);
    lbl = new QLabel("");
    lbl->setObjectName("kfreq"+QString::number(i));
    lbl->setStyleSheet(ValuesFormat);
    glyout->addWidget(lbl,8,1,1,2);
    lbl=new QLabel("Kinter:");
    lbl->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    glyout->addWidget(lbl,8,3,1,1);
    lbl = new QLabel("");
    lbl->setObjectName("kinter"+QString::number(i));
    lbl->setStyleSheet(ValuesFormat);
    glyout->addWidget(lbl,8,4,1,2);

    QPushButton *pb = new QPushButton("Прочитать настроечные коэффициенты из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
    glyout->addWidget(pb, 9, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в модуль");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefs()));
    glyout->addWidget(pb, 10, 0, 1, 6);
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFromFile()));
    glyout->addWidget(pb, 11, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    glyout->addWidget(pb, 12, 0, 1, 6);
    pb = new QPushButton("Запустить связь с МИП");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMip()));
//    pb->setEnabled(false);
    glyout->addWidget(pb, 13, 0, 1, 6);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    cp1->setLayout(lyout);

/*    lyout = new QVBoxLayout;
    pb = new QPushButton("Начать настройку");
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
    cp2->setLayout(lyout); */
    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
}

void e_tunedialog::StartTune()
{

}

/*void e_tunedialog::tune(int tunenum)
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
            cn->Send(CN_Gda, &Bda0, sizeof(Bda));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        CheckAndShowTune0();
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
            cn->Send(CN_Gda, &Bda5, sizeof(Bda));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        CheckAndShowTune5();
        }
        break;
    }
    case 20:
    {
        if (QMessageBox::information(this,"Настройка",\
                                     "Переключите входные переключатели на ток,\nустановите ток 20 мА на всех" \
                                     "\nвходах модуля и нажмите OK") == QMessageBox::Ok)
        {
            cn->Send(CN_Gda, &Bda20, sizeof(Bda));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        CheckAndShowTune20();
        }
        break;
    }
    default:
        break;
    }
}

void e_tunedialog::tune0()
{
    tune(0);
}

void e_tunedialog::CheckAndShowTune0()
{
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

void e_tunedialog::tune5()
{
    tune(5);
}

void e_tunedialog::CheckAndShowTune5()
{
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

void e_tunedialog::tune20()
{
    tune(20);
}

void e_tunedialog::CheckAndShowTune20()
{
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
*/
void e_tunedialog::ReadTuneCoefs()
{
    cn->Send(CN_Gac, &Bac_block, sizeof(Bac_block));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        RefreshTuneCoefs();
}

void e_tunedialog::WriteTuneCoefs()
{
    cn->Send(CN_Wac, &Bac_block, sizeof(Bac_block));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        QMessageBox::information(this,"Успешно!","Записано успешно!");
}

void e_tunedialog::CalcNewTuneCoefs()
{
    for (int i = 0; i < 16; i++)
    {
        Bac_block[i].fbin = 1.25 - (static_cast<float>(Bda0.sin[i])/(Z*1638.0));
        if ((Bda0.sin[i] == Bda5.sin[i]) || (Bda0.sin[i] == Bda20.sin[i]))
        {
            QMessageBox::warning(this,"Предупреждение","Ошибки в настроечных коэффициентах\nДеление на ноль");
            return;
        }
        Bac_block[i].fkuin = Z*1638.0/static_cast<float>(Bda0.sin[i]-Bda5.sin[i]);
        Bac_block[i].fkiin = Z*1638.0/static_cast<float>(Bda0.sin[i]-Bda20.sin[i]);
    }
}

void e_tunedialog::RefreshTuneCoefs()
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

void e_tunedialog::LoadFromFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Открыть файл", ".", "Configuration (*.conf)");
    if (filename.isEmpty())
        return;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit error(ER_FILEOPENERROR);
        return;
    }
    QByteArray *ba = new QByteArray(file.readAll());
    publicclass::Bsi Bsi_block;
    if (ba->size() >= (sizeof(publicclass::Bsi)+sizeof(Bac_block)))
    {
        memcpy(&Bsi_block,ba,sizeof(publicclass::Bsi));
        if ((Bsi_block.CpuIdHigh != pc.CpuIdHigh) || (Bsi_block.SerNum != pc.SerNum))
        {
            if (QMessageBox::question(this,"Не тот файл","В файле содержатся данные для модуля с другим CPUID и/или SN.\nПродолжить загрузку?",\
                                      QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok);
            else
                return;
        }
        // продолжение загрузки файла
    }
}

void e_tunedialog::SaveToFile()
{

}

void e_tunedialog::StartMip()
{
    mipcanal = new iec104;
    connect(mipcanal,SIGNAL(error(int)),this,SIGNAL(error(int)));
    connect(mipcanal,SIGNAL(readdatafrometh(QByteArray)),this,SLOT(MipDataRcv(QByteArray)));
    connect(mipcanal,SIGNAL(writedatatoeth(QByteArray)),this,SLOT(MipDataXmit(QByteArray)));
    connect(mipcanal,SIGNAL(signalsreceived()),this,SLOT(MipData()));
    connect(mipcanal,SIGNAL(ethconnected()),this,SLOT(EthConnected()));
    connect(mipcanal,SIGNAL(startack()),this,SLOT(MipConnected()));
}

void e_tunedialog::EthConnected()
{
    QByteArray ba;
    ba.clear();
    ba.insert(0,0x3A);
    emit dataready(ba);
}

void e_tunedialog::MipConnected()
{
    QMessageBox::warning(this,"w","Связь с МИП-02 установлена!");
}

void e_tunedialog::MipDataRcv(QByteArray ba)
{
//    ba.insert(0,'<');
//    emit dataready(ba);
}

void e_tunedialog::MipDataXmit(QByteArray ba)
{
//    ba.insert(0,'>');
//    emit dataready(ba);
}

void e_tunedialog::MipData()
{
    // приём из mipcanal::Signals номеров сигналов (SigNum) и их значений (SigVal) и их дальнейшая обработка
}

void e_tunedialog::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}
