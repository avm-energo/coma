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

#include "publicclass.h"
#include "confdialog.h"

confdialog::confdialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    Bci_block = new Bci;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    lyout->addWidget(ConfTW);
    setLayout(lyout);
}

void confdialog::SetChTypData(int num, s_tqComboBox *cb)
{
    Bci_block->in_type[cb->getAData().toInt()] = num;
}

void confdialog::GetBci()
{
    QByteArray tmpba = ">GF";
    tmpba.resize(4);
    tmpba[3] = 0x01;
    connect(pc.SThread,SIGNAL(receivecompleted()),this,SLOT(FillConfData()));
    pc.SThread->InitiateWriteDataToPort(tmpba);
}

void confdialog::FillConfData()
{
    if (BciUnpack())
    {
        int i, j;
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
        default:
        {
            ConfTW->addTab(cp1,"Каналы");
            ConfTW->addTab(cp2,"Уставки");
            ConfTW->addTab(cp3,"Общие");
            QStringList ChTypSl = QStringList() << "Не исп." << "мА" << "В";
            QStringListModel *ChTypSlM = new QStringListModel;
            ChTypSlM->setStringList(ChTypSl);
            int row = 0;
            for (i = 0; i < pc.ANumCh1(); i++)
            {
                QLabel *ChTypL = new QLabel("Тип канала "+QString::number(i)+":");
                s_tqComboBox *ChTypCB = new s_tqComboBox;
                ChTypCB->setAData(QVariant(i));
                ChTypCB->setObjectName("chtypcb"+QString::number(i));
                connect(ChTypCB,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetChTypData(int,s_tqComboBox*)));
                lyout1->addWidget(ChTypL, row, i*2, 1, 1);
                lyout1->addWidget(ChTypCB, row, 1*2+1, 1, 1);
                if ((i>0)&&(i%8))
                    row++;
            }
            for (j = 0; j < pc.ANumCh2(); j++)
            {
                QLabel *ChTypL = new QLabel("Тип канала "+QString::number(j)+":");
                s_tqComboBox *ChTypCB = new s_tqComboBox;
                ChTypCB->setAData(QVariant(j));
                ChTypCB->setObjectName("chtypcb"+QString::number(j));
                connect(ChTypCB,SIGNAL(textChanged(int,s_tqComboBox*)),this,SLOT(SetChTypData(int,s_tqComboBox*)));
                lyout1->addWidget(ChTypL, row, j*2+i, 1, 1);
                lyout1->addWidget(ChTypCB, row, j*2+i+1, 1, 1);
            }
            break;
        }
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
    //    default:
    //        break;
        }
        cp1->setLayout(lyout1);
    }
    else
    {
        QMessageBox::warning(this,"warning!","Принят неправильный блок конфигурации Bciu");
    }
}

int confdialog::BciPack()
{
    return 0;
}

int confdialog::BciUnpack()
{
    return 0;
}
