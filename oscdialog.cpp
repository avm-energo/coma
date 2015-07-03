#include "oscdialog.h"
#include "canal.h"
#include "widgets/s_tqtableview.h"
#include "widgets/s_tablemodel.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QProgressBar>
#include <QApplication>
#include <QDateTime>

oscdialog::oscdialog(QWidget *parent) :
    QDialog(parent)
{
    SetupUI();
}

void oscdialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    s_tqTableView *tv = new s_tqTableView;
    tv->setObjectName("osctv");
    tm = new s_tablemodel;
    tv->setModel(tm);
    QPushButton *pb = new QPushButton("Получить данные по осциллограммам в памяти модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(GetOscInfo()));
    lyout->addWidget(pb);
    lyout->addWidget(tv, 89);
    lyout->addStretch(1);
    setLayout(lyout);
}

void oscdialog::GetOscInfo()
{
    connect(cn,SIGNAL(incomingdatalength(quint32)),this,SLOT(SetProgressBarSize(quint32)));
    connect(cn,SIGNAL(bytesreceived(quint32)),this,SLOT(SetProgressBar(quint32)));
    OscInfo = new QByteArray;
    OscInfo->resize(30000);
    cn->Send(CN_GNosc,&(OscInfo->data()[0]));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        ProcessOscInfo();
}

void oscdialog::ProcessOscInfo()
{
    emit endprogressbar();
    QList<QStringList> lsl;
    tm->addColumn("#");
    tm->addColumn("Канал");
    tm->addColumn("Дата/Время");
    quint32 OscInfoSize = cn->RDSize;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QStringList Num, Cha, Tim;
    for (quint32 i = 0; i < OscInfoSize; i+= 16)
    {
        quint32 oscnum = static_cast<quint32>(OscInfo->at(i));
        oscnum += static_cast<quint32>(OscInfo->at(i+1))*256;
        quint32 chnum = static_cast<quint32>(OscInfo->at(i+4));
        chnum += static_cast<quint32>(OscInfo->at(i+5))*256;
        quint32 unixtime = static_cast<quint32>(OscInfo->at(i+8));
        unixtime += static_cast<quint32>(OscInfo->at(i+9))*256;
        unixtime += static_cast<quint32>(OscInfo->at(i+10))*65536;
        unixtime += static_cast<quint32>(OscInfo->at(i+11))*16777216;
        quint32 timens = static_cast<quint32>(OscInfo->at(i+12));
        timens += static_cast<quint32>(OscInfo->at(i+13))*256;
        timens += static_cast<quint32>(OscInfo->at(i+14))*65536;
        timens += static_cast<quint32>(OscInfo->at(i+15))*16777216;
        QDateTime tme = QDateTime::fromTime_t(unixtime);
        QString ms = QString::number((timens/1000000));
        QString mcs = QString::number(((timens-ms.toInt()*1000000)/1000));
        QString ns = QString::number(timens-ms.toInt()*1000000-mcs.toInt()*1000);
//        QStringList sl = QStringList() << QString::number(oscnum) << QString::number(chnum) << \
//                                          tme.toString("dd/MM/yyyy hh:mm:ss.")+ms+"."+mcs+"."+ns;
        Num << QString::number(oscnum);
        Cha << QString::number(chnum);
        Tim << tme.toString("dd/MM/yyyy hh:mm:ss.")+ms+"."+mcs+"."+ns;
    }
    lsl.append(Num);
    lsl.append(Cha);
    lsl.append(Tim);
    tm->fillModel(lsl);
    QApplication::restoreOverrideCursor();
    s_tqTableView *tv = this->findChild<s_tqTableView *>("osctv");
    if (tv == 0)
        return;
    tv->resizeRowsToContents();
    tv->resizeColumnsToContents();
    connect(tv,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(BeginExtractOsc(QModelIndex)));
}

void oscdialog::SetProgressBar(quint32 cursize)
{
    QProgressBar *prb = this->findChild<QProgressBar *>("oscprb");
    if (prb != 0)
        prb->setValue(cursize);
}

void oscdialog::SetProgressBarSize(quint32 size)
{
    QWidget *wdgt = new QWidget;
    wdgt->setAttribute(Qt::WA_DeleteOnClose);
    wdgt->setWindowModality(Qt::WindowModal);
    connect(this,SIGNAL(endprogressbar()),wdgt,SLOT(close()));
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Загрузка данных...");
    lyout->addWidget(lbl,0,Qt::AlignTop);
    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("oscprb");
    prb->setOrientation(Qt::Vertical);
    prb->setMinimum(0);
    prb->setMaximum(size);
    lyout->addWidget(prb);
    wdgt->setVisible(true);
}

void oscdialog::BeginExtractOsc(QModelIndex index)
{
    quint32 oscnum = tm->data(index.sibling(index.row(),0),Qt::DisplayRole).toInt();
    OscInfo->clear();
    cn->Send(CN_GBosc,&(OscInfo->data()[0]),0,oscnum);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        EndExtractOsc();
}

void oscdialog::EndExtractOsc()
{
}