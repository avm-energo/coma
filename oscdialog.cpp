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
#include <QMessageBox>

oscdialog::oscdialog(QWidget *parent) :
    QDialog(parent)
{
    SetupUI();
    OscInfo = 0;
}

oscdialog::~oscdialog()
{
    if (OscInfo != 0)
        delete OscInfo;
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
    if (OscInfo != 0)
        delete OscInfo;
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
    {
        OSCDBG;
        return; // !!! системная проблема
    }
    QItemSelectionModel *m = tv->selectionModel();
    tv->setModel(tm);
    delete m;
    GetOscPBDelegate *dg = new GetOscPBDelegate;
    connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(GetOsc(QModelIndex)));
    tv->setItemDelegateForColumn(3,dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    tv->resizeRowsToContents();
    tv->resizeColumnsToContents();
}

void oscdialog::EndExtractOsc()
{
    quint32 OscLength = static_cast<quint8>(OscInfo->at(0));
    OscLength += static_cast<quint8>(OscInfo->at(1))*256;
    OscLength += static_cast<quint8>(OscInfo->at(2))*65536;
    OscLength += static_cast<quint8>(OscInfo->at(3))*16777216;
    QByteArray OscData;
    switch(pc.MType)
    {
    case MT_A:
    {
        OscData.resize(OscLength-24); // 12 = DataRec(0xFFFF)
        Config[0] = {MT_A_OSCTYPE, float_TYPE, sizeof(float), (OscLength-24)/sizeof(float),OscData.data()};
        // 24=sizeof(dataHeader-block)+sizeof(DataRec FFFF-block)
        Config[1] = {0xFFFF, 0, 0, 0, NULL};
        int res = pc.RestoreDataMem(OscInfo->data(),OscLength+12,Config); // 12 = +DataHeader
        if (res)
        {
            if (res < pc.errmsgs.size())
                OSCER(pc.errmsgs.at(res));
            else
                OSCER("Произошла неведомая фигня #"+QString::number(res,10));
            return; // !!! ошибка разбора формата С2
        }
        // обработка принятой осциллограммы и запись её в файл
        int WRow = 6;
        int WCol = 1;
        memcpy(&OscHeader,OscData.data(),sizeof(OscHeader)); // скопировали информацию об осциллограмме в структуру
        OscData.remove(0,sizeof(OscHeader)); // и удалили её из входного буфера, чтобы осталась только сама осциллограмма
        OscData.chop(12); // последний Datarec (0xFFFF) долой
        if (OscHeader.NsTime > 999999999L)
            OscHeader.NsTime = 0; // !!! ошибка - число наносекунд не может быть больше 999 млн 999 тыс 999
        GivenFilename.insert(0, pc.ModuleTypeString+" "+QString::number(OscHeader.ChNum)+"-");
        QString filename = QFileDialog::getSaveFileName(this,"Сохранить осциллограмму",GivenFilename,"Excel files (*.xlsx)");
        if (filename == "")
        {
            OSCER("Не задано имя файла");
            return; // !!! ошибка - не задано имя файла
        }
        QXlsx::Document xlsx(filename.toUtf8());
        float StartTime = 0; // нулевое смещение относительно начала записи осциллограммы
        xlsx.write(1,1,QVariant("Модуль: "+pc.ModuleTypeString));
        xlsx.write(2,1,QVariant("Канал: "+QString::number(OscHeader.ChNum)));
        xlsx.write(3,1,QVariant("Дата: "+OscDateTime.split(" ").at(0)));
        xlsx.write(4,1,QVariant("Время: "+OscDateTime.split(" ").at(1)));
        xlsx.write(5,1,QVariant("Смещение, нс"));
        xlsx.write(5,2,QVariant("Значение по каналу " + QString::number(OscHeader.ChNum)));
        quint32 i = 0; // указатель внутри OscData
        quint32 OscDataSize = OscData.size();
        float tmpf;
        float PtPerInMs = OscHeader.PtPer/1000000.0f;
        while (i < OscDataSize)
        {
            QString tmps = QString::number(StartTime,'g',6);
            xlsx.write(WRow,WCol,QVariant(tmps));
            memcpy(&tmpf,&(OscData.data()[i]),sizeof(tmpf));
            xlsx.write(WRow,WCol+1,QVariant(tmpf));
            WRow++;
            StartTime += PtPerInMs;
            i+=4; // отсчёт++
        }
        xlsx.save();
        QMessageBox::information(this,"Успешно!","Записано успешно!");
        break;
    }
    case MT_E:
    {
        OscData.resize(OscLength-36);
        Config[0] = {MT_E_OSCTYPE, float_TYPE, sizeof(float), (OscLength-36)/sizeof(float),OscData.data()};
        // 24=sizeof(dataHeader-block)+sizeof(DataRec FFFF-block)
        Config[1] = {0xFFFF, 0, 0, 0, NULL};
        int res = pc.RestoreDataMem(OscInfo->data(),OscLength+12,Config); // 12 = +DataHeader
        if (res)
        {
            if (res < pc.errmsgs.size())
                OSCER(pc.errmsgs.at(res));
            else
                OSCER("Произошла неведомая фигня #"+QString::number(res,10));
            return; // !!! ошибка разбора формата С2
        }
        // обработка принятой осциллограммы и запись её в файл
        int WRow = 6;
        int WCol = 1;
        memcpy(&OscHeader,OscData.data(),sizeof(OscHeader)); // скопировали информацию об осциллограмме в структуру
        OscData.remove(0,sizeof(OscHeader)); // и удалили её из входного буфера, чтобы осталась только сама осциллограмма
        OscData.chop(12); // последний Datarec (0xFFFF) долой
        if (OscHeader.NsTime > 999999999L)
            OscHeader.NsTime = 0; // !!! ошибка - число наносекунд не может быть больше 999 млн 999 тыс 999
        GivenFilename.insert(0, pc.ModuleTypeString+" "+QString::number(OscHeader.ChNum)+"-");
        QString filename = QFileDialog::getSaveFileName(this,"Сохранить осциллограмму",GivenFilename,"Excel files (*.xlsx)");
        if (filename == "")
        {
            OSCER("Не задано имя файла");
            return; // !!! ошибка - не задано имя файла
        }
        QXlsx::Document xlsx(filename.toUtf8());
        float StartTime = 0; // нулевое смещение относительно начала записи осциллограммы
        xlsx.write(1,1,QVariant("Модуль: "+pc.ModuleTypeString));
        xlsx.write(2,1,QVariant("Канал: "+QString::number(OscHeader.ChNum)));
        xlsx.write(3,1,QVariant("Дата: "+OscDateTime.split(" ").at(0)));
        xlsx.write(4,1,QVariant("Время: "+OscDateTime.split(" ").at(1)));
        xlsx.write(5,1,QVariant("Смещение, нс"));
        for (quint32 j=0; j<OscHeader.Qty; j++)
            xlsx.write(5,j+2,QVariant("Значение по каналу " + QString::number(j)));
        quint32 i = 0; // указатель внутри OscData
        quint32 OscDataSize = OscData.size();
        float tmpf;
        quint32 OneOscLenInBytes = OscHeader.Len*4;
        float PtPerInMs = OscHeader.PtPer/1000000.0f;
        quint32 iFinish = OscDataSize / OscHeader.Qty;
        while (i < iFinish) // минус один отсчёт во float минус размер одного последнего DataRec-а
        {
            QString tmps = QString::number(StartTime,'g',6);
            xlsx.write(WRow,WCol,QVariant(tmps));
            for (quint32 j=0; j<OscHeader.Qty; j++)
            {
                memcpy(&tmpf,&(OscData.data()[i+j*OneOscLenInBytes]),sizeof(tmpf));
                xlsx.write(WRow,WCol+j+1,QVariant(tmpf));
            }
            WRow++;
            StartTime += PtPerInMs;
            i+=4; // отсчёт++
        }
        xlsx.save();
        QMessageBox::information(this,"Успешно!","Записано успешно!");
        break;
    }
    default:
        break;
    }

}

void oscdialog::GetOsc(QModelIndex idx)
{
//    quint32 oscnum = tm->data(index.sibling(index.row(),0),Qt::DisplayRole).toInt();
    quint32 oscnum = tm->data(idx.sibling(idx.row(),0),Qt::DisplayRole).toInt(); // номер осциллограммы
    OscDateTime = tm->data(idx.sibling(idx.row(),2),Qt::DisplayRole).toString(); // дата и время создания осциллограммы
//    OscInfo = new QByteArray;
    OscInfo->resize(40000);
    GivenFilename = OscDateTime;
    GivenFilename.replace("/","-");
    GivenFilename.replace(":","_");
    GivenFilename.insert(0, " ");
    GivenFilename.insert(0, QString::number(oscnum));
    cn->Send(CN_GBosc,OscInfo->data(),0,oscnum);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        EndExtractOsc();
}
