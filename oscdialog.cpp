#include "oscdialog.h"
#include "canal.h"
#include "widgets/s_tqtableview.h"
#include "widgets/s_tablemodel.h"
#include "widgets/getoscpbdelegate.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QProgressBar>
#include <QApplication>
#include <QDateTime>
#include <QtXlsx/xlsxdocument.h>
#include <QFileDialog>

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
    disconnect(cn,SIGNAL(incomingdatalength(quint32)),this,SLOT(SetProgressBarSize(quint32)));
    disconnect(cn,SIGNAL(bytesreceived(quint32)),this,SLOT(SetProgressBar(quint32)));
    emit endprogressbar();
    QList<QStringList> lsl;
    tm->ClearModel();
    tm->addColumn("#");
    tm->addColumn("Канал");
    tm->addColumn("Дата/Время");
    tm->addColumn("Скачать");
    quint32 OscInfoSize = cn->RDSize;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QStringList Num, Cha, Tim, Dwld;
    for (quint32 i = 0; i < OscInfoSize; i+= 16)
    {
        quint32 oscnum = static_cast<quint8>(OscInfo->at(i));
        oscnum += static_cast<quint8>(OscInfo->at(i+1))*256;
        quint32 chnum = static_cast<quint8>(OscInfo->at(i+4));
        chnum += static_cast<quint8>(OscInfo->at(i+5))*256;
        quint32 unixtime = static_cast<quint8>(OscInfo->at(i+8));
        unixtime += static_cast<quint8>(OscInfo->at(i+9))*256;
        unixtime += static_cast<quint8>(OscInfo->at(i+10))*65536;
        unixtime += static_cast<quint8>(OscInfo->at(i+11))*16777216;
        quint32 timens = static_cast<quint8>(OscInfo->at(i+12));
        timens += static_cast<quint8>(OscInfo->at(i+13))*256;
        timens += static_cast<quint8>(OscInfo->at(i+14))*65536;
        timens += static_cast<quint8>(OscInfo->at(i+15))*16777216;
        QDateTime tme = QDateTime::fromTime_t(unixtime);
        QString ms = QString::number((timens/1000000));
        QString mcs = QString::number(((timens-ms.toInt()*1000000)/1000));
        QString ns = QString::number(timens-ms.toInt()*1000000-mcs.toInt()*1000);
//        QStringList sl = QStringList() << QString::number(oscnum) << QString::number(chnum) << \
//                                          tme.toString("dd/MM/yyyy hh:mm:ss.")+ms+"."+mcs+"."+ns;
        Num << QString::number(oscnum);
        Cha << QString::number(chnum);
        Tim << tme.toString("dd/MM/yyyy hh:mm:ss.")+ms+"."+mcs+"."+ns;
        Dwld << "Скачать";
    }
    lsl.append(Num);
    lsl.append(Cha);
    lsl.append(Tim);
    lsl.append(Dwld);
    tm->fillModel(lsl);
    QApplication::restoreOverrideCursor();
    s_tqTableView *tv = this->findChild<s_tqTableView *>("osctv");
    if (tv == 0)
        return; // !!! системная проблема
    tv->setModel(tm);
    GetOscPBDelegate *dg = new GetOscPBDelegate;
    connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(GetOsc(QModelIndex)));
    tv->setItemDelegateForColumn(3,dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    tv->resizeRowsToContents();
    tv->resizeColumnsToContents();
}

void oscdialog::SetProgressBar(quint32 cursize)
{
    QProgressBar *prb = this->findChild<QProgressBar *>("oscprb");
    if (prb != 0)
        prb->setValue(cursize);
}

void oscdialog::SetProgressBarSize(quint32 size)
{
    QDialog *dlg = new QDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowModality(Qt::WindowModal);
    connect(this,SIGNAL(endprogressbar()),dlg,SLOT(close()));
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Загрузка данных...");
    lyout->addWidget(lbl,0,Qt::AlignTop);
    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("oscprb");
    prb->setOrientation(Qt::Vertical);
    prb->setMinimum(0);
    prb->setMaximum(size);
    lyout->addWidget(prb);
    dlg->setLayout(lyout);
    dlg->setVisible(true);
}

void oscdialog::EndExtractOsc()
{
    quint32 OscLength = static_cast<quint8>(OscInfo->at(0));
    OscLength += static_cast<quint8>(OscInfo->at(1))*256;
    OscLength += static_cast<quint8>(OscInfo->at(2))*65536;
    OscLength += static_cast<quint8>(OscInfo->at(3))*16777216;
    QByteArray *OscData = new QByteArray;
    switch(pc.MType)
    {
    case MT_A:
    {
        Config[0] = {MT_A_OSCTYPE, float_TYPE, sizeof(float), (OscLength-24)/sizeof(quint8),OscData->data()}; // 24=sizeof(DataHeader)+sizeof(DataRec)
        Config[1] = {0xFFFF, 0, 0, 0, NULL};
        OscData->resize(OscLength);
        int res = pc.RestoreDataMem(OscInfo->data(),OscLength,Config);
        if (res)
            return; // !!! ошибка разбора формата С2
        break;
    }
    case MT_E:
    {
        break;
    }
    default:
        break;
    }
    // обработка принятой осциллограммы и запись её в файл
    QString filename = QFileDialog::getSaveFileName(this,"Сохранить осциллограмму",GivenFilename,"Excel files (*.xlsx)");
    if (filename == "")
        return; // !!! ошибка - не задано имя файла
    QXlsx::Document xlsx(filename.toUtf8());
    int WRow = 2;
    int WCol = 2;
    memcpy(&OscHeader,OscData->data(),sizeof(OscHeader)); // скопировали информацию об осциллограмме в структуру
    OscData->remove(0,sizeof(OscHeader)); // и удалили её из входного буфера, чтобы осталась только сама осциллограмма
    if (OscHeader.NsTime > 999999999L)
        OscHeader.NsTime = 0; // !!! ошибка - число наносекунд не может быть больше 999 млн 999 тыс 999
    quint64 StartTime = static_cast<quint64>(OscHeader.UnixTime)*1000000000L + OscHeader.NsTime;
    xlsx.write(1,1,QVariant("Модуль "+pc.ModuleTypeString));
    xlsx.write(1,2,QVariant("Канал "+QString::number(OscHeader.ChNum)));
    xlsx.write(1,3,QVariant("Дата "+pc.NsTimeToString(StartTime).split(" ").at(0)));
    xlsx.write(1,4,QVariant("Время "+pc.NsTimeToString(StartTime).split(" ").at(1)));
    quint32 i = 0; // указатель внутри OscData
    quint32 OscDataSize = OscData->size();
    float tmpf;
    while (i < (OscDataSize-3)) // минус один отсчёт во float
    {
        QString tmps = pc.NsTimeToString(StartTime);
        xlsx.write(WRow,WCol,QVariant(tmps));
        memcpy(&tmpf,OscData->data(),sizeof(tmpf));
        xlsx.write(WRow,WCol+1,QVariant(tmpf));
        WRow++;
        StartTime += OscHeader.PtPer;
    }
    xlsx.save();
}

void oscdialog::GetOsc(QModelIndex idx)
{
//    quint32 oscnum = tm->data(index.sibling(index.row(),0),Qt::DisplayRole).toInt();
    quint32 oscnum = tm->data(idx.sibling(idx.row(),0),Qt::DisplayRole).toInt(); // номер осциллограммы
    QString OscDateTime = tm->data(idx.sibling(idx.row(),2),Qt::DisplayRole).toString(); // дата и время создания осциллограммы
    OscInfo = new QByteArray;
    OscInfo->resize(30000);
    GivenFilename = QString::number(oscnum)+"-"+OscDateTime;
    GivenFilename.replace("/","-");
    GivenFilename.replace(":","_");
    cn->Send(CN_GBosc,&(OscInfo->data()[0]),0,oscnum);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        EndExtractOsc();
}
