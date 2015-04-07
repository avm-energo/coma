#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QMessageBox>

#include "confdialog.h"

a_confdialog::a_confdialog(QWidget *parent) :
    QDialog(parent)
{
    Config[0] = {BCI_INTYPE, u8_TYPE, sizeof(qint8), sizeof(Bci_block.in_type)/sizeof(qint8), &(Bci_block.in_type)};
    Config[1] = {BCI_INMIN, float_TYPE, sizeof(float), sizeof(Bci_block.in_min)/sizeof(float), &(Bci_block.in_min)};
    Config[2] = {BCI_INMAX, float_TYPE, sizeof(float), sizeof(Bci_block.in_max)/sizeof(float), &Bci_block.in_max};
    Config[3] = {BCI_INVMIN, float_TYPE, sizeof(float), sizeof(Bci_block.in_vmin)/sizeof(float), &Bci_block.in_vmin};
    Config[4] = {BCI_INVMAX, float_TYPE, sizeof(float), sizeof(Bci_block.in_vmax)/sizeof(float), &Bci_block.in_vmax};
    Config[5] = {BCI_SETMINMIN, float_TYPE, sizeof(float), sizeof(Bci_block.setminmin)/sizeof(float), &Bci_block.setminmin};
    Config[6] = {BCI_SETMIN, float_TYPE, sizeof(float), sizeof(Bci_block.setmin)/sizeof(float), &Bci_block.setmin};
    Config[7] = {BCI_SETMAX, float_TYPE, sizeof(float), sizeof(Bci_block.setmax)/sizeof(float), &Bci_block.setmax};
    Config[8] = {BCI_SETMAXMAX, float_TYPE, sizeof(float), sizeof(Bci_block.setmaxmax)/sizeof(float), &Bci_block.setmaxmax};
    Config[9] = {BCI_DISCOSC, u16_TYPE, sizeof(quint16), sizeof(Bci_block.discosc)/sizeof(quint16), &Bci_block.discosc};
    Config[10] = {BCI_OSCSRC, u32_TYPE, sizeof(quint32), sizeof(Bci_block.oscsrc)/sizeof(quint32), &Bci_block.oscsrc};
    Config[11] = {BCI_OSCDLY, u16_TYPE, sizeof(quint16), sizeof(Bci_block.oscdly)/sizeof(quint16), &Bci_block.oscdly};
    Config[12] = {BCI_CTYPE, u16_TYPE, sizeof(quint16), sizeof(Bci_block.Ctype)/sizeof(quint16), &Bci_block.Ctype};
    Config[13] = {BCI_ABS_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Abs_104)/sizeof(quint32), &Bci_block.Abs_104};
    Config[14] = {BCI_CYCLE_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Cycle_104)/sizeof(quint32), &Bci_block.Cycle_104};
    Config[15] = {BCI_T1_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T1_104)/sizeof(quint32), &Bci_block.T1_104};
    Config[16] = {BCI_T2_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T2_104)/sizeof(quint32), &Bci_block.T2_104};
    Config[17] = {BCI_T3_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T3_104)/sizeof(quint32), &Bci_block.T3_104};
    Config[18] = {BCI_K_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.k_104)/sizeof(quint32), &Bci_block.k_104};
    Config[19] = {BCI_W_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.w_104)/sizeof(quint32), &Bci_block.w_104};
    Config[20] = {0xFFFF, 0, 0, 0, NULL};
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    lyout->addWidget(ConfTW);
    setLayout(lyout);
    GetBci();
}

void a_confdialog::GetBci()
{
    QByteArray tmpba = ">GF";
    tmpba.resize(4);
    tmpba[3] = 0x01;
    connect(pc.SThread,SIGNAL(receivecompleted()),this,SLOT(FillConfData()));
    pc.SThread->InitiateWriteDataToPort(tmpba);
    FillConfData(); // temporary
}

void a_confdialog::FillConfData()
{
    int i, j;
    disconnect(pc.SThread,SIGNAL(receivecompleted()),this,SLOT(FillConfData()));
    QGridLayout *lyout1 = new QGridLayout;
    QGridLayout *lyout2 = new QGridLayout;
    QGridLayout *lyout3 = new QGridLayout;
    QGridLayout *lyout4 = new QGridLayout;
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QWidget *cp4 = new QWidget;
    QTabWidget *ConfTW = this->findChild<QTabWidget *>("conftw");
    if (ConfTW == 0)
        return;
    switch (pc.MType)
    {
    case MT_A:
    {
        int res;
        QByteArray *ba = new QByteArray(pc.SThread->data());
        if ((ba->at(0) != 0x3c) && (ba->at(1) != 0x01))
            return;
        ba->remove(0, 2);
        if (!(res = pc.RestoreDataMem(ba->data(), ba->size(), Config)))
        {
            ConfTW->addTab(cp1,"Общие");
            ConfTW->addTab(cp2,"Диапазоны");
            ConfTW->addTab(cp3,"Уставки");
            ConfTW->addTab(cp4,"104");
            QGroupBox *gb = new QGroupBox("Типы каналов");
            QVBoxLayout *gblyout = new QVBoxLayout;
            QHBoxLayout *gb2lyout = new QHBoxLayout;
            QStringList ChTypSl = QStringList() << "Не исп." << "мА" << "В";
            QStringListModel *ChTypSlM = new QStringListModel;
            ChTypSlM->setStringList(ChTypSl);
            for (i = 0; i < 16; i++)
            {
                QLabel *ChTypL = new QLabel(QString::number(i)+":");
                s_tqComboBox *ChTypCB = new s_tqComboBox;
                ChTypCB->setModel(ChTypSlM);
                ChTypCB->setAData(QVariant(i));
                ChTypCB->setObjectName("chtypcb"+QString::number(i));
                ChTypCB->setCurrentIndex(Bci_block.in_type[i]);
                connect(ChTypCB,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetChTypData(int,s_tqComboBox*)));
                gb2lyout->addWidget(ChTypL);
                gb2lyout->addWidget(ChTypCB, 1);
                if ((i>1)&&!((i+1)%4))
                {
                    gblyout->addLayout(gb2lyout);
                    gb2lyout = new QHBoxLayout;
                }
            }
            gb->setLayout(gblyout);
            lyout1->addWidget(gb, 0, 0, 1, 1);
            gb = new QGroupBox("Осциллограммы");
            gblyout = new QVBoxLayout;
            QGridLayout *gb3lyout = new QGridLayout;
            QLabel *lbl = new QLabel("Каналы записи осциллограмм:");
            gblyout->addWidget(lbl);
            for (i=0; i<16; i++)
            {
                lbl=new QLabel(QString::number(i));
                s_tqCheckBox *chb = new s_tqCheckBox;
                chb->setText("");
                chb->setAData(QVariant(i));
                if (Bci_block.discosc & (0x0001 << i))
                    chb->setChecked(true);
                else
                    chb->setChecked(false);
                connect(chb,SIGNAL(statechanged(int,s_tqCheckBox*)),this,SLOT(SetChOsc(int,s_tqCheckBox*)));
                gb3lyout->addWidget(lbl, 0, i, 1, 1, Qt::AlignCenter);
                gb3lyout->addWidget(chb,1,0,1,1,Qt::AlignCenter);
            }
            gblyout->addLayout(gb3lyout);
            ChTypSl = QStringList() << "Ком. Ц" << "U>" << "DI" << "Любой";
            ChTypSlM = new QStringListModel;
            ChTypSlM->setStringList(ChTypSl);
            lbl = new QLabel("События-инициаторы запуска осциллограмм:");
            gblyout->addWidget(lbl);
            gb2lyout = new QHBoxLayout;
            for (i = 0; i < 16; i++)
            {
                QLabel *ChTypL = new QLabel(QString::number(i)+":");
                s_tqComboBox *ChTypCB = new s_tqComboBox;
                ChTypCB->setModel(ChTypSlM);
                ChTypCB->setAData(QVariant(i));
                ChTypCB->setCurrentIndex(Bci_block.oscsrc&(0x00000011 << i));
                connect(ChTypCB,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetChOscSrc(int,s_tqComboBox*)));
                gb2lyout->addWidget(ChTypL);
                gb2lyout->addWidget(ChTypCB, 1);
                if ((i>1)&&!((i+1)%4))
                {
                    gblyout->addLayout(gb2lyout);
                    gb2lyout = new QHBoxLayout;
                }
            }
            gblyout->addLayout(gblyout);
            lbl = new QLabel("Задержка в мс начала фиксации максимумов:");
            gblyout->addWidget(lbl);
            QSpinBox *spb = new QSpinBox;
            spb->setSingleStep(1);
            spb->setValue(Bci_block.oscdly);
            spb->setMaximum(10000);
            connect(spb,SIGNAL(valueChanged(int)),this,SLOT(SetOscDly(int)));
            gblyout->addWidget(spb);
            gb->setLayout(gblyout);
            lyout1->addWidget(gb, 0, 0, 1, 1);
        }
        else
        {
            QMessageBox::warning(this,"warning!","Принят неправильный блок конфигурации Bciu, ошибка "+QString::number(res));
        }
    }
//    default:
//        break;
    case MT_C:
    {
        break;
    }
    case MT_D:
    {
        break;
    }
    case MT_E:
    {
        break;
    }
    default:
        break;
    }
    cp1->setLayout(lyout1);
}

void a_confdialog::SetChTypData(int num, s_tqComboBox *cb)
{
    Bci_block.in_type[cb->getAData().toInt()] = num;
}

void a_confdialog::SetOscDly(int dly)
{
    Bci_block.oscdly = dly;
}

void a_confdialog::SetChOsc(int isChecked, s_tqCheckBox *ptr)
{
    quint16 tmpint = 0x0001;
    tmpint = tmpint << ptr->getAData().toInt();
    if (isChecked == Qt::Checked)
        Bci_block.discosc |= tmpint;
    else
        Bci_block.discosc &= ~tmpint;
}

void a_confdialog::SetChOscSrc(int srctyp, s_tqComboBox *ptr)
{
    quint32 tmpint = srctyp << ptr->getAData().toInt();
    quint32 tmpmask = ~(0x00000011 << ptr->getAData().toInt());
    Bci_block.oscsrc &= tmpmask;
    Bci_block.oscsrc |= tmpint;
}
