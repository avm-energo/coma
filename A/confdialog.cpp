#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QCheckBox>
#include <QSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QMessageBox>

#include "confdialog.h"

a_confdialog::a_confdialog(QWidget *parent) :
    QDialog(parent)
{
//    Config = new publicclass::DataRec[18];
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
    Config[10] = {BCI_CTYPE, u16_TYPE, sizeof(quint16), sizeof(Bci_block.Ctype)/sizeof(quint16), &Bci_block.Ctype};
    Config[11] = {BCI_ABS_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Abs_104)/sizeof(quint32), &Bci_block.Abs_104};
    Config[12] = {BCI_CYCLE_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.Cycle_104)/sizeof(quint32), &Bci_block.Cycle_104};
    Config[13] = {BCI_T1_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T1_104)/sizeof(quint32), &Bci_block.T1_104};
    Config[14] = {BCI_T2_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T2_104)/sizeof(quint32), &Bci_block.T2_104};
    Config[15] = {BCI_T3_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.T3_104)/sizeof(quint32), &Bci_block.T3_104};
    Config[16] = {BCI_K_104, u32_TYPE, sizeof(quint32), sizeof(Bci_block.k_104)/sizeof(quint32), &Bci_block.k_104};
    Config[17] = {0xFFFF, 0, 0, 0, NULL};
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
//    FillConfData(); // temporary
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
            ConfTW->addTab(cp1,"Каналы");
            ConfTW->addTab(cp2,"Уставки");
            ConfTW->addTab(cp3,"Общие");
            QGroupBox *gb = new QGroupBox("Типы каналов");
            QVBoxLayout *gblyout = new QVBoxLayout;
            QHBoxLayout *gb2lyout = new QHBoxLayout;
            QStringList ChTypSl = QStringList() << "Не исп." << "мА" << "В";
            QStringListModel *ChTypSlM = new QStringListModel;
            ChTypSlM->setStringList(ChTypSl);
// temporary            for (i = 0; i < pc.ANumCh1(); i++)
            for (i = 0; i < 16; i++)
            {
                QLabel *ChTypL = new QLabel(QString::number(i)+":");
                s_tqComboBox *ChTypCB = new s_tqComboBox;
                ChTypCB->setModel(ChTypSlM);
                ChTypCB->setAData(QVariant(i));
                ChTypCB->setObjectName("chtypcb"+QString::number(i));
// temporary                ChTypCB->setCurrentIndex(Bci_block.in_type[i]);
                ChTypCB->setCurrentIndex(0); // temporary
                connect(ChTypCB,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetChTypData(int,s_tqComboBox*)));
                gb2lyout->addWidget(ChTypL);
                gb2lyout->addWidget(ChTypCB, 1);
                if ((i>1)&&!((i+1)%4))
                {
                    gblyout->addLayout(gb2lyout);
                    gb2lyout = new QHBoxLayout;
                }
            }
/* temporary            for (j = 0; j < pc.ANumCh2(); j++)
            {
                QLabel *ChTypL = new QLabel("к"+QString::number(j)+":");
                s_tqComboBox *ChTypCB = new s_tqComboBox;
                ChTypCB->setModel(ChTypSlM);
                ChTypCB->setAData(QVariant(j));
                ChTypCB->setObjectName("chtypcb"+QString::number(j));
                ChTypCB->setCurrentIndex(Bci_block.in_type[j]);
                connect(ChTypCB,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetChTypData(int,s_tqComboBox*)));
                gb2lyout->addWidget(ChTypL);
                gb2lyout->addWidget(ChTypCB, 1);
                if ((j>1)&&!((j+1)%4))
                    gblyout->addLayout(gb2lyout);
            } */
            for (i = 0; i < 16; i++)
            {
                QLabel *ChTypL = new QLabel(QString::number(i)+":");
                s_tqComboBox *ChTypCB = new s_tqComboBox;
                ChTypCB->setModel(ChTypSlM);
                ChTypCB->setAData(QVariant(i));
                ChTypCB->setObjectName("chtypcb"+QString::number(i));
// temporary                ChTypCB->setCurrentIndex(Bci_block.in_type[i]);
                ChTypCB->setCurrentIndex(0); // temporary
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
