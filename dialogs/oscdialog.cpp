#include "oscdialog.h"
#include "canal.h"
#include "config.h"
#include "widgets/s_tqtableview.h"
#include "widgets/s_tablemodel.h"
#include "widgets/getoscpbdelegate.h"
#include "widgets/messagebox.h"
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
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    lyout->addWidget(tv, 89);
    pb = new QPushButton("Стереть все осциллограммы в памяти модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(EraseOsc()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    setLayout(lyout);
}

void oscdialog::GetOscInfo()
{
    if (OscInfo != 0)
        delete OscInfo;
    OscInfo = new QByteArray;
    OscInfo->resize(MAXOSCBUFSIZE);
    cn->Send(CN_GBo,Canal::BT_NONE,&(OscInfo->data()[0]));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == NOERROR)
        ProcessOscInfo();
}

void oscdialog::ProcessOscInfo()
{
/*    QList<QStringList> lsl;
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
        Num << QString::number(oscnum);
        Tim << tme.toString("dd/MM/yyyy hh:mm:ss.")+ms+"."+mcs+"."+ns;
        Dwld << "Скачать";
    }
    lsl.append(Num);
    lsl.append(Tim);
    lsl.append(Dwld);
    tm->fillModel(lsl);
    QApplication::restoreOverrideCursor();
    s_tqTableView *tv = this->findChild<s_tqTableView *>("osctv");
    if (tv == 0)
    {
        DBGMSG;
        return; // !!! системная проблема
    }
    QItemSelectionModel *m = tv->selectionModel();
    tv->setModel(tm);
    delete m;
    GetOscPBDelegate *dg = new GetOscPBDelegate;
    connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(GetOsc(QModelIndex)));
    tv->setItemDelegateForColumn(2,dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    tv->resizeRowsToContents();
    tv->resizeColumnsToContents(); */
}

void oscdialog::EndExtractOsc()
{
    quint32 j;
    quint32 OscLength = static_cast<quint8>(OscInfo->at(0));
    OscLength += static_cast<quint8>(OscInfo->at(1))*256;
    OscLength += static_cast<quint8>(OscInfo->at(2))*65536;
    OscLength += static_cast<quint8>(OscInfo->at(3))*16777216;
    if (OscLength > static_cast<quint32>(OscInfo->size()))
    {
        ERMSG("Передана неверная длина в блоке FileHeader");
        return;
    }
    // считываем шапку - информацию о передаваемом файле осциллограмм - в структуру OscHeader
    Config[0] = {MT_HEAD_ID, sizeof(OscHeader),&OscHeader};
    Config[1] = {0xFFFFFFFF, 0, NULL};
    int res = pc.RestoreDataMem(OscInfo->data(),OscLength+12,Config); // 12 = +FileHeader
    if (res)
        return; // !!! ошибка разбора формата С2
    QByteArray OscData; // буфер для складирования осциллограмм
    quint32 OscSize = OscHeader.Len*sizeof(float); // размер одной осциллограммы в байтах
    quint32 OscNum = (OscLength-12-sizeof(OscHeader_Data)) / OscSize; // рассчитаем количество осциллограмм
    OscData.resize(OscSize*OscNum); // отведём в буфере место под осциллограммы
    float tmpf;
    switch(pc.ModuleBsi.MTypeB)
    {
    case MTB_21:
    {
        quint32 IDFound = 0;
        for (j=MT_START_OSC; j<MT_END_OSC; j++) // цикл по возможным осциллограммам
        {
            Config[0] = {j&0xFFFFFFFF, OscSize, &(OscData.data()[0])};
            Config[1] = {0xFFFFFFFF, 0, NULL};
            int res = pc.RestoreDataMem(OscInfo->data(),OscLength+12,Config); // 12 = +FileHeader
            if (!res)
            {
                IDFound = j;
                break;
            }
            else if (res == S2_NOIDS)
                continue;
            else
            {
                if (res < pc.errmsgs.size())
                    ERMSG(pc.errmsgs.at(res));
                else
                    ERMSG("Произошла неведомая фигня #"+QString::number(res,10));
                return; // !!! ошибка разбора формата С2
            }
        }
        if (!IDFound) // так и не найдено ни одного ID
        {
            ERMSG("Не найдено ни одного известного ID в принятом блоке осциллограмм");
            return;
        }
        // обработка принятой осциллограммы и запись её в файл
        if (OscHeader.NsTime > 999999999L)
            OscHeader.NsTime = 0; // !!! ошибка - число наносекунд не может быть больше 999 млн 999 тыс 999
        GivenFilename.insert(0, pc.ModuleTypeString+"-");
        QString filename = QFileDialog::getSaveFileName(this,"Сохранить осциллограмму",GivenFilename,"Excel files (*.xlsx)");
        if (filename == "")
        {
            ERMSG("Не задано имя файла");
            return; // !!! ошибка - не задано имя файла
        }
        QXlsx::Document xlsx(filename.toUtf8());
        xlsx.write(1,1,QVariant("Модуль: "+pc.ModuleTypeString));
        xlsx.write(3,1,QVariant("Дата: "+OscDateTime.split(" ").at(0)));
        xlsx.write(4,1,QVariant("Время: "+OscDateTime.split(" ").at(1)));
        xlsx.write(5,1,QVariant("Смещение, мс"));
        xlsx.write(5,2,QVariant("Значение по каналу " + QString::number(IDFound-MT_START_OSC)));
        quint32 i = 0; // указатель внутри OscData
        float tmpf;
        int WRow = 6;
        int WCol = 1;
        float StartTime = 0; // нулевое смещение относительно начала записи осциллограммы
        while (i < OscHeader.Len)
        {
            xlsx.write(WRow,WCol,QVariant(StartTime));
            memcpy(&tmpf,&(OscData.data()[i*4]),sizeof(tmpf));
            xlsx.write(WRow,WCol+1,QVariant(tmpf));
            WRow++;
            StartTime += OscHeader.Step;
            i++; // отсчёт++
        }
        xlsx.save();
        QMessageBox::information(this,"Успешно!","Записано успешно!");
        break;
    }
    case MTB_80:
    {
        // обработка принятой осциллограммы и запись её в файл
        if (OscHeader.NsTime > 999999999L)
            OscHeader.NsTime = 0; // !!! ошибка - число наносекунд не может быть больше 999 млн 999 тыс 999
        GivenFilename.insert(0, pc.ModuleTypeString+"-");
        QString filename = QFileDialog::getSaveFileName(this,"Сохранить осциллограмму",GivenFilename,"Excel files (*.xlsx)");
        if (filename == "")
        {
            ERMSG("Не задано имя файла");
            return; // !!! ошибка - не задано имя файла
        }
        QXlsx::Document xlsx(filename.toUtf8());
        float StartTime = 0; // нулевое смещение относительно начала записи осциллограммы
        xlsx.write(1,1,QVariant("Модуль: "+pc.ModuleTypeString));
        xlsx.write(3,1,QVariant("Дата: "+OscDateTime.split(" ").at(0)));
        xlsx.write(4,1,QVariant("Время: "+OscDateTime.split(" ").at(1)));
        xlsx.write(5,1,QVariant("Смещение, мс"));
        quint32 i = 0; // указатель внутри OscData
        int WRow = 6;
        int WCol = 1;
        // подготовим временную шкалу в первом столбце
        while (i < OscHeader.Len) // минус один отсчёт во float минус размер одного последнего DataRec-а
        {
            xlsx.write(WRow,WCol,QVariant(StartTime));
            WRow++;
            StartTime += OscHeader.Step;
            i++;
        }

        for (j=0; j<OscNum; j++) // цикл по возможным осциллограммам
        {
            Config[0] = {(j+MT_START_OSC)&0xFFFFFFFF, OscSize, &(OscData.data()[j*OscSize])};
            Config[1] = {0xFFFFFFFF, 0, NULL};
            int res = pc.RestoreDataMem(OscInfo->data(),OscLength+12,Config); // 12 = +FileHeader
            if (!res)
            {
                WRow = 5;
                WCol = j+2;
                xlsx.write(WRow,WCol,QVariant("Канал " + QString::number(j)));
                WRow++;
                i = 0;
                quint32 OscOffset = OscSize * j;
                while (i < OscHeader.Len)
                {
                    memcpy(&tmpf,&(OscData.data()[OscOffset+i*sizeof(float)]),sizeof(tmpf));
                    xlsx.write(WRow,WCol,QVariant(tmpf));
                    WRow++;
                    i++; // отсчёт++
                }
            }
            else if (res == S2_NOIDS)
            {
                MessageBox2::information(this, "Внимание", "Не найден блок с ID = "+QString::number(j+MT_START_OSC));
                continue;
            }
            else
            {
                if (res < pc.errmsgs.size())
                    ERMSG(pc.errmsgs.at(res));
                else
                    ERMSG("Произошла неведомая фигня #"+QString::number(res,10));
                return; // !!! ошибка разбора формата С2
            }
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
    quint32 oscnum = tm->data(idx.sibling(idx.row(),0),Qt::DisplayRole).toInt(); // номер осциллограммы
    OscDateTime = tm->data(idx.sibling(idx.row(),1),Qt::DisplayRole).toString(); // дата и время создания осциллограммы
    OscInfo->resize(MAXOSCBUFSIZE);
    GivenFilename = OscDateTime;
    GivenFilename.replace("/","-");
    GivenFilename.replace(":","_");
    GivenFilename.insert(0, " ");
    GivenFilename.insert(0, QString::number(oscnum));
    cn->Send(CN_GF,Canal::BT_NONE,OscInfo->data(),0,oscnum);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == NOERROR)
        EndExtractOsc();
}

void oscdialog::EraseOsc()
{
    pc.PrbMessage = "Стёрто записей: ";
    cn->Send(CN_OscEr);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == NOERROR)
        MessageBox2::information(this, "Внимание", "Стёрто успешно");
    else
        MessageBox2::information(this, "Внимание", "Ошибка при стирании");
}