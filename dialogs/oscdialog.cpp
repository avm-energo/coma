#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QProgressBar>
#include <QApplication>
#include <QDateTime>
#include "QtXlsx/xlsxdocument.h"
#include <QFileDialog>
#include <QMessageBox>
#include "oscdialog.h"
#include "../commands.h"
#include "../config/config.h"
#include "trendviewdialog.h"
#include "../widgets/s_tqtableview.h"
#include "../widgets/etablemodel.h"
#include "../widgets/getoscpbdelegate.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

OscDialog::OscDialog(QWidget *parent) :
    QDialog(parent)
{
    SetupUI();
}

OscDialog::~OscDialog()
{
}

void OscDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    s_tqTableView *tv = new s_tqTableView;
    tv->setObjectName("osctv");
    tm = new ETableModel;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = new QPushButton("Получить данные по осциллограммам в памяти "+tmps);
    connect(pb,SIGNAL(clicked()),this,SLOT(GetAndProcessOscInfo()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    lyout->addWidget(tv, 89);
    pb = new QPushButton("Стереть все осциллограммы в памяти "+tmps);
    connect(pb,SIGNAL(clicked()),this,SLOT(EraseOsc()));
// !!!   if (pc.Emul)
// !!!       pb->setEnabled(false);
    lyout->addWidget(pb);
    setLayout(lyout);
}

bool OscDialog::PosPlusPlus(void *dst, void *src, int size)
{
    BSize = size;
    if (Pos > (BASize-BSize))
    {
        ERMSG("pos > ba.size");
        return false;
    }
    memcpy(dst, src, BSize);
    Pos += BSize;
    return true;
}

int OscDialog::InputFileType()
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Вывод осциллограмм"), Qt::AlignCenter);
    lyout->addWidget(WDFunc::NewChB(this, "xlsxchb", "В файл .xlsx"));
    lyout->addWidget(WDFunc::NewChB(this, "comtradechb", "В файлы COMTRADE"));
    lyout->addWidget(WDFunc::NewChB(this, "nonechb", "На экран"));
    QPushButton *pb = new QPushButton("Продолжить");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Accept()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Cancel()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    AcceptedOscFileType = false;
    pc.Cancelled = false;
    dlg->show();
    while (!AcceptedOscFileType && !pc.Cancelled)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < SLEEPINT)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    dlg->close();
    if (pc.Cancelled)
        return GENERALERROR;
    return NOERROR;
}

void OscDialog::GetAndProcessOscInfo()
{
    QVector<QVector<QVariant> > lsl;
    QByteArray OscInfo;
    quint32 OscInfoSize; // размер считанного буфера с информацией об осциллограммах
    quint32 RecordSize = sizeof(GBoStruct); // GBo struct size
    OscInfoSize = MAXOSCBUFSIZE;
    OscInfo.resize(OscInfoSize);
    if ((Commands::GetBo(&(OscInfo.data()[0]), OscInfoSize)) == NOERROR)
    {
        if (OscInfoSize < RecordSize)
        {
            MessageBox2::information(this, "Информация", "Присланное количество байт слишком мало");
            return;
        }
        tm->ClearModel();
        tm->addColumn("#");
        tm->addColumn("ИД");
        tm->addColumn("Дата/Время");
        tm->addColumn("Длина");
        tm->addColumn("Скачать");
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QVector<QVariant> Num, IDs, Tim, Lngth, Dwld;
        int counter = 0;
        for (quint32 i = 0; i < OscInfoSize; i+= RecordSize)
        {
            GBoStruct gbos;
            memcpy(&gbos, &(OscInfo.data()[i]), RecordSize);
            Num << QString::number(gbos.FileNum);
            Tim << pc.UnixTime64ToString(gbos.UnixTime);
            IDs << gbos.ID;
            Lngth << gbos.FileLength;
            ++counter;
        }
        Dwld.fill("Скачать", counter);
        lsl.append(Num);
        lsl.append(IDs);
        lsl.append(Tim);
        lsl.append(Lngth);
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
        tv->setItemDelegateForColumn(4,dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
        tv->resizeRowsToContents();
        tv->resizeColumnsToContents();
    }
}

void OscDialog::GetOsc(QModelIndex idx)
{
    bool ok;
    quint32 oscnum = tm->data(idx.sibling(idx.row(),0),Qt::DisplayRole).toInt(&ok); // номер осциллограммы
    if (!ok)
    {
        WARNMSG("");
        return;
    }
    quint32 oscid = tm->data(idx.sibling(idx.row(),1),Qt::DisplayRole).toInt(&ok); // ИД осциллограммы (9000, 9001 ...)
    if (!ok)
    {
        WARNMSG("");
        return;
    }
    OscDateTime = tm->data(idx.sibling(idx.row(),2),Qt::DisplayRole).toString(); // дата и время создания осциллограммы
    BASize = tm->data(idx.sibling(idx.row(),3),Qt::DisplayRole).toInt(&ok);
    if (!ok)
    {
        WARNMSG("");
        return;
    }
//    OscLength -= sizeof(publicclass::DataRec) + sizeof(OscHeader_Data); // длина данных по осциллограммам без заголовков
//    OscLength /= oscid; // считаем, что все осциллограммы в файле одинаковой длины, и нет повторяющихся ИД
//    OscInfo.resize(MAXOSCBUFSIZE);
    BA.resize(BASize);
    if (Commands::GetOsc(oscnum, &(BA.data()[0])) == NOERROR)
    {
        if (InputFileType() == GENERALERROR)
        {
            MessageBox2::error(this, "Ошибка", "Ошибка при задании типа файла осциллограмм");
            return;
        }
        Pos = 0;
        // разбираем осциллограмму
        publicclass::FileHeader FH;
        if (!PosPlusPlus(&FH, &(BA.data()[Pos]), sizeof(publicclass::FileHeader)))
            return;
        // проводим проверку, то ли получили
        bool isOk = ((FH.fname == oscnum) && (FH.size == BASize));
        if (!isOk)
        {
            MessageBox2::error(this, "Ошибка", "Данные об осциллограмме не совпадают с заявленными");
            return;
        }
        OscDataRec DR;
        if (!PosPlusPlus(&DR, &(BA.data()[Pos]), sizeof(OscDataRec)))
            return;
        if (DR.id != oscid)
        {
            MessageBox2::error(this, "Ошибка", "Данные об осциллограмме не совпадают с заявленными");
            return;
        }
        oscid -= MT_HEAD_ID - 1; // одна осциллограмма = 1, две = 2, ...
        if (oscid > 8)
            oscid = 1; // если что-то с количеством осциллограмм не так, принудительно выставляем в 1
        OscHeader_Data OHD;
        if (!PosPlusPlus(&OHD, &(BA.data()[Pos]), sizeof(OscHeader_Data)))
            return;
        for (quint32 i=0; i<oscid; ++i)
        {
            if (!PosPlusPlus(&DR, &(BA.data()[Pos]), sizeof(OscDataRec)))
                return;
            // составляем имя файла осциллограммы
            GivenFilename = pc.UnixTime64ToString(OHD.unixtime);
            GivenFilename.replace("/","-");
            GivenFilename.replace(":","_");
            GivenFilename.insert(0, "_");
            GivenFilename.insert(0, QString::number(i));
            GivenFilename.insert(0, "_");
            GivenFilename.insert(0, QString::number(DR.id));
            // пишем саму осциллограмму
            EndExtractOsc(DR.id, OHD);
        }
    }
    else
        WARNMSG("");
}

void OscDialog::EndExtractOsc(quint32 id, OscHeader_Data &OHD)
{
    QString FileName = pc.HomeDir + "/" + GivenFilename;
    QXlsx::Document xlsx(FileName);
    if (OscFileType & MT_FT_XLSX)
    {
        xlsx.write(1,1,QVariant("Модуль: "+pc.ModuleTypeString));
        xlsx.write(2,1,QVariant("Дата: "+OscDateTime.split(" ").at(0)));
        xlsx.write(3,1,QVariant("Время: "+OscDateTime.split(" ").at(1)));
        xlsx.write(4,1,QVariant("Смещение, мс"));
    }
    if ((id >= MT_ID21) && (id <= MT_ID21E))
    {
        // осциллограмма 21 модуля
        /*quint32 IDFound = 0;
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
        return; */
    }
    else
    {
        switch(id)
        {
        case MT_ID81:
        case MT_ID82:
        case MT_ID83:
        {
            /*if (OscHeader.NsTime > 999999999L)
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
            break; */
        }
        case MT_ID85:
        {
            TrendViewDialog *dlg = new TrendViewDialog;
            if (OscFileType & MT_FT_XLSX)
            {
                for (int col = 2; col < 5; ++col)
                {
                    QString phase = QString::number(8+col, 16); // A,B,C
                    xlsx.write(4,col,QVariant("OCN"+phase));
                    xlsx.write(4,col+3,QVariant("OCF"+phase));
                    xlsx.write(4,col+6,QVariant("BKC"+phase));
                    xlsx.write(4,col+9,QVariant("BKO"+phase));
                    xlsx.write(4,col+14,QVariant("CN"+phase));
                    xlsx.write(4,col+17,QVariant("CF"+phase));
                    xlsx.write(4,col+20,QVariant("US"+phase));
                    xlsx.write(4,col+23,QVariant("I"+phase));
                    xlsx.write(4,col+26,QVariant("UL"+phase));
                }
                xlsx.write(4, 12, QVariant("CSC"));
                xlsx.write(4, 13, QVariant("CSO"));
            }
            if (OscFileType & MT_FT_NONE)
            {
                QVector<QString> tmpdv, tmpav;
                tmpdv << "OCNA" << "OCNB" << "OCNC" << "OCFA" << "OCFB" << "OCFC" << \
                         "BKCA" << "BKCB" << "BKCC" << "BKOA" << "BKOB" << "BKOC" << \
                         "CSC" << "CSO" << "CNA" << "CNB" << "CNC" << "CFA" << "CFB" << "CFC";
                tmpav << "USA" << "USB" << "USC" << "IA" << "IB" << "IC" << "ULA" << "ULB" << "ULC";
                dlg->Init(tmpdv, tmpav, OHD.len, -200, 200);
                dlg->SetPointsAxis(0, OHD.step);
            }
            int row = 5;
            for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
            {
                Point85 point;
                if (i == (OHD.len-1))
                    i = OHD.len - 1;
                if (!PosPlusPlus(&point, &(BA.data()[Pos]), sizeof(Point85)))
                    return;
                int col = 2; // 2 = OCNA
                quint32 DisPoint = point.Dis & 0x000FFFFF; // оставляем только младшие 20 бит
                if (OscFileType & MT_FT_XLSX)
                {
                    for (int i=0; i<20; ++i)
                    {
                        if (DisPoint & 0x00000001)
                            xlsx.write(row, col++, QVariant("1"));
                        else
                            xlsx.write(row, col++, QVariant("0"));
                        DisPoint >>= 1;
                    }
                    col = 22;
                    while (col < 31)
                        xlsx.write(row, col++, QVariant(QString::number(point.An[col-22], 'f', 6)));
                }
                DisPoint = point.Dis & 0x000FFFFF; // оставляем только младшие 20 бит
                int count = 0; // номер графика
                if (OscFileType & MT_FT_NONE)
                {
                    for (int i=0; i<20; ++i)
                    {
                        if (DisPoint & 0x00000001)
                            dlg->AddDigitalPoint(count, 1);
                        else
                            dlg->AddDigitalPoint(count, 0);
                        DisPoint >>= 1;
                        ++count;
                    }
                    for (count = 0; count < 9; ++count)
                        dlg->AddAnalogPoint(count, point.An[count]);
                }
            }
            if (OscFileType & MT_FT_XLSX)
                xlsx.save();
            if (OscFileType & MT_FT_NONE)
            {
                dlg->PlotShow();
                dlg->exec();
            }
            break;
        }
        default:
            break;
        }
    }
}

void OscDialog::EraseOsc()
{
/* !!!   pc.PrbMessage = "Стёрто записей: ";
    if (Commands::EraseOsc() == NOERROR)
        MessageBox2::information(this, "Внимание", "Стёрто успешно");
    else
        MessageBox2::information(this, "Внимание", "Ошибка при стирании"); */
}

void OscDialog::Accept()
{
    OscFileType = 0;
    bool isChecked;
    WDFunc::ChBData(this, "xlsxchb", isChecked);
    if (isChecked)
        OscFileType |= MT_FT_XLSX;
    WDFunc::ChBData(this, "comtradechb", isChecked);
    if (isChecked)
        OscFileType |= MT_FT_COMTRADE;
    WDFunc::ChBData(this, "nonechb", isChecked);
    if (isChecked)
        OscFileType |= MT_FT_NONE;
    AcceptedOscFileType = true;
}

void OscDialog::Cancel()
{
    pc.Cancelled = true;
}
