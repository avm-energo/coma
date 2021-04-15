#include "eoscillogram.h"

#include "../gen/colors.h"
#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "parseid10031.h"
#include "parseid9000.h"
#include "parseid9050.h"
#include "parsemodule.h"
#include "trendviewdialog.h"
#include "xlsxdocument.h"

#include <QVector>
#include <cstring>

EOscillogram::EOscillogram(QObject *parent) : QObject(parent)
{
    PointsNum = 0;
}

EOscillogram::~EOscillogram()
{
}

/*void EOscillogram::AddAnalogPoint(int GraphNum, float PointValue)
{
    if (GraphNum < AnalogMainData.size())
    {
        QVector<double> tmpv = AnalogMainData.at(GraphNum);
        tmpv.append(PointValue);
        AnalogMainData.replace(GraphNum, tmpv);
    }
}

void EOscillogram::AddDigitalPoint(int GraphNum, int PointValue)
{
    if (GraphNum < DigitalMainData.size())
    {
        QVector<double> tmpv = DigitalMainData.at(GraphNum);
        tmpv.append(PointValue);
        DigitalMainData.replace(GraphNum, tmpv);
    }
} */

void EOscillogram::SaveToExcel()
{
    /*    if (Filename.isEmpty())
        {
            DBGMSG;
            return;
        }
        QStringList sl = Filename.split("#"); // отделяем имя файла от даты-времени
        if (sl.size() < 2)
        {
            DBGMSG;
            return;
        }
        Filename = sl.at(0);
        QString OscDateTime = sl.at(1);
        QString tmps = pc.HomeDir + "/";
        Filename.insert(0, tmps);
        QXlsx::Document xlsx(Filename);
        xlsx.write(1,1,QVariant("Модуль: "+pc.ModuleTypeString));
        xlsx.write(2,1,QVariant("Дата: "+OscDateTime.split(" ").at(0)));
        xlsx.write(3,1,QVariant("Время: "+OscDateTime.split(" ").at(1)));
        xlsx.write(4,1,QVariant("Смещение, мс"));
        quint32 MTypeB = pc.ModuleBsi.MTypeB << 8;
        switch(MTypeB)
        {
        case MTB_85:
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
            int row = 5;
            for (int i = 0; i < MainPoints.size(); ++i) // цикл по точкам
            {
                int col = 2; // 2 = OCNA
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
            break;
        }
        default:
            break;
        }
            xlsx.save(); */
    /* из 8х
                 if (OscHeader.NsTime > 999999999L)
                OscHeader.NsTime = 0; // !!! ошибка - число наносекунд не может быть больше 999 млн 999 тыс 999
            GivenFilename.insert(0, pc.ModuleTypeString+"-");
            QString filename = QFileDialog::getSaveFileName(this,"Сохранить осциллограмму",GivenFilename,"Excel files
       (*.xlsx)"); if (filename == "")
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
                    EMessageBox::information(this, "Внимание", "Не найден блок с ID =
       "+QString::number(j+MT_START_OSC)); continue;
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
    QString filename = QFileDialog::getSaveFileName(this,"Сохранить осциллограмму",GivenFilename,"Excel files
    (*.xlsx)"); if (filename == "")
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

void EOscillogram::SaveToComtrade()
{
}

// void EOscillogram::SetFilename(const QString &fn)
//{
//    Filename = fn;
//}

bool EOscillogram::PosPlusPlus(void *dst, int size)
{
    BSize = size;
    if (Pos > (BASize - BSize))
    {
        qCritical("pos > ba.size");
        return false;
    }
    memcpy(dst, &(BA.data()[Pos]), BSize);
    Pos += BSize;
    return true;
}

bool EOscillogram::ProcessOsc(TrendViewModel *mdl)
{
    Pos = 0;
    // разбираем осциллограмму
    S2DataTypes::FileHeader FH;

    BASize = (BA.size() + sizeof(FH));
    // EOscillogram::SWJournalRecordStruct SWJRecord;

    if (Pos > BASize)
        return false;
    memcpy(&FH, &(BA.data()[0]), sizeof(FH));

    Pos += sizeof(FH);
    S2DataTypes::DataRecHeader DR;
    if (Pos > BASize)
        return false;
    memcpy(&DR, &(BA.data()[Pos]), sizeof(DR));

    while (DR.id != 0xFFFFFFFF)
    {
        if (Pos >= BASize)
            break;

        std::unique_ptr<ParseModule> parseModule;
        Pos += sizeof(DR);
        switch (DR.id)
        {
        case MT_HEAD_ID:
            parseModule = std::unique_ptr<ParseModule>(new ParseID9000(BA));
            // PM = new ParseID9000(BA);
            break;

        case MT_HEAD87:
            parseModule = std::unique_ptr<ParseModule>(new ParseID9050(BA));
            // PM = new ParseID9050(BA);
            break;

        case SWJ_ID85:
        {
            memcpy(&SWJRecord, &(BA.data()[Pos]), sizeof(S2DataTypes::SwitchJourRecord));
            if (DR.numByte == sizeof(S2DataTypes::SwitchJourRecord))
            {
                Pos += sizeof(S2DataTypes::SwitchJourRecord);
                parseModule = std::unique_ptr<ParseModule>(new ParseID10031(BA));
                // PM = new ParseID10031(BA);
            }
            else
                return false;
            break;
        }
        default:
            return false;
        }
        if (!parseModule->Parse(Pos))
            return false;

        if (Pos > BASize)
        {
            qWarning() << Error::SizeError;
            return false;
        }
        memcpy(&DR, &(BA.data()[Pos]), sizeof(DR));
        Pos += sizeof(DR);
        *mdl = *(parseModule->trendViewModel());
    }
    /*
    Pos = 0;
    BASize = BA.size();
    // разбираем осциллограмму
    S2::FileHeader FH;

    if (!PosPlusPlus(&FH, sizeof(S2::FileHeader)))
        return Error::ER_GENERALERROR;

    quint32 id = 0;
    quint32 oscid = 0;
    if (!PosPlusPlus(&id, sizeof(id)))
        return Error::ER_GENERALERROR;
    switch (id) {

       case MT_HEAD_ID:

        OscDataRec DR;
        if (!PosPlusPlus(&DR.numbytes, (sizeof(OscDataRec)-sizeof(id))))
            return Error::ER_GENERALERROR;
       oscid = DR.id;

        oscid -= MT_HEAD_ID - 1; // одна осциллограмма = 1, две = 2, ...
        if (oscid > 8)
            oscid = 1; // если что-то с количеством осциллограмм не так, принудительно выставляем в 1
        OscHeader_Data OHD;
        if (!PosPlusPlus(&OHD, sizeof(OscHeader_Data)))
            return Error::ER_GENERALERROR;
        for (quint32 i=0; i<oscid; ++i)
        {
            if (!PosPlusPlus(&DR, sizeof(OscDataRec)))
                return Error::ER_GENERALERROR;

            TrendViewModel::SaveID(DR.id); // для выбора
            // составляем имя файла осциллограммы
            QString tmps = TimeFunc::UnixTime64ToString(OHD.unixtime);
            tmps.replace("/","-");
            tmps.replace(":","_");
            tmps.insert(0, "_");
            tmps.insert(0, QString::number(i));
            tmps.insert(0, "_");
            tmps.insert(0, QString::number(DR.id));
            // пишем саму осциллограмму
            if (ProcessOneOsc(DR.id, OHD, tmps) != Error::ER_NOERROR)
                return Error::ER_GENERALERROR;
        }




        break;

       case SWJ_ID85:

        DataRecSwj Sw;
        Sw.id = id;
        if (!PosPlusPlus(&Sw.numbytes, (sizeof(DataRecSwj)-sizeof(id))))
            return Error::ER_GENERALERROR;


       break;

       case MT_ID87:


        break;

    }


*/
    return true;
}

/*int EOscillogram::ProcessOneOsc(quint32 id, EOscillogram::OscHeader_Data &OHD, const QString &fn)
{
    QStringList tmpav, tmpdv;
    TrendViewDialog *dlg = new TrendViewDialog(BA);
    if ((id >= MT_ID21) && (id <= MT_ID21E))
    {
        // период отсчётов - 20 мс, длительность записи осциллограммы 10 сек, итого 500 точек по 4 байта на каждую
        tmpav << QString::number(id); // пока сделано для одного канала в осциллограмме
        TrendViewModel *TModel = new TrendViewModel(QStringList(), tmpav, OHD.len);
        dlg->SetModel(TModel);
        dlg->SetAnalogNames(tmpav);
        dlg->SetRanges(0, 10000, -20, 20); // 10000 мс, 20 мА (сделать автонастройку в зависимости от конфигурации по
данному каналу) dlg->SetupPlots(id); dlg->SetupUI(); if (!TModel->SetPointsAxis(0, OHD.step)) return
Error::ER_GENERALERROR; for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
        {
            Point21 point;
            if (!PosPlusPlus(&point, sizeof(Point21)))
                return Error::ER_GENERALERROR;
            TModel->AddAnalogPoint(tmpav.at(0), point.An);
        }
        TModel->SetFilename(fn);
        dlg->setModal(false);
        dlg->PlotShow();
        dlg->show();
    }
    else
    {
        switch(id)
        {
        case MT_ID80:
        {
            tmpav << "UA" << "UB" << "UC" << "IA" << "IB" << "IC";
            tmpdv.clear();
//            int np = C80
            float xmax = (static_cast<float>(OHD.len/2));
            float xmin = -xmax;
            xmin = -(OHD.step * 512);
            TrendViewModel *TModel = new TrendViewModel(tmpdv, tmpav, OHD.len);
            dlg->SetModel(TModel);
            dlg->SetAnalogNames(tmpav);
            dlg->SetDigitalNames(tmpdv);
            dlg->SetRanges(xmin, xmax, -200, 200);
            dlg->SetupPlots(id);
            dlg->SetupUI();
            if (!TModel->SetPointsAxis(xmin, OHD.step))
                return Error::ER_GENERALERROR;
            for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
            {
                Point8x point;
                if (!PosPlusPlus(&point, sizeof(Point8x)))
                    return Error::ER_GENERALERROR;
                for (int i=0; i<6; ++i)
                    TModel->AddAnalogPoint(tmpav.at(i), point.An[i]);
            }
            TModel->SetFilename(fn);
            dlg->setModal(false);
            dlg->PlotShow();
            dlg->show();
            break;
        }
        case MT_ID85:
        {
            tmpdv << "OCNA" << "OCNB" << "OCNC" << "OCFA" << "OCFB" << "OCFC" << \
                     "BKCA" << "BKCB" << "BKCC" << "BKOA" << "BKOB" << "BKOC" << \
                     "CSC" << "CSO" << "CNA" << "CNB" << "CNC" << "CFA" << "CFB" << "CFC" << \
                     "nNA" << "nNB" << "nNC" << "nFA" << "nFB" << "nFC" << "nCA" << "nCB" << "nCC" << \
                     "nOA" << "nOB" << "nOC";
            tmpav << "USA" << "USB" << "USC" << "IA" << "IB" << "IC" << "ULA" << "ULB" << "ULC";
            float xmax = (static_cast<float>(OHD.len/2));
            float xmin = -xmax;
            xmin = -(OHD.step * 512);
            TrendViewModel *TModel = new TrendViewModel(tmpdv, tmpav, OHD.len);
            dlg->SetModel(TModel);
            dlg->SetAnalogNames(tmpav);
            dlg->SetDigitalNames(tmpdv);
            QStringList acolors = QStringList() << YLWCOLOR << GRNCOLOR << REDCOLOR << \
                                                   YLLCOLOR << GRLCOLOR << RDLCOLOR << \
                                                   YLDCOLOR << GRDCOLOR << RDDCOLOR;
            QStringList dcolors = QStringList() << YLWCOLOR << GRNCOLOR << REDCOLOR << \
                                                   YLLCOLOR << GRLCOLOR << RDLCOLOR << \
                                                   YLDCOLOR << GRDCOLOR << RDDCOLOR << \
                                                   YLWCOLOR << GRNCOLOR << REDCOLOR << \
                                                   BLUCOLOR << BLDCOLOR << \
                                                   YLLCOLOR << GRLCOLOR << RDLCOLOR << \
                                                   YLDCOLOR << GRDCOLOR << RDDCOLOR << \
                                                   YLWCOLOR << GRNCOLOR << REDCOLOR << \
                                                   YLLCOLOR << GRLCOLOR << RDLCOLOR << \
                                                   YLDCOLOR << GRDCOLOR << RDDCOLOR << \
                                                   YLWCOLOR << GRNCOLOR << REDCOLOR;
            dlg->SetDigitalColors(dcolors);
            dlg->SetAnalogColors(acolors);
            dlg->SetRanges(xmin, xmax, -200, 200);
            dlg->SetupPlots(id);
            dlg->SetupUI();
            if (!TModel->SetPointsAxis(xmin, OHD.step))
                return Error::ER_GENERALERROR;
            for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
            {
                Point85 point;
                if (!PosPlusPlus(&point, sizeof(Point85)))
                    return Error::ER_GENERALERROR;
//                quint32 DisPoint = point.Dis & 0x000FFFFF; // оставляем только младшие 20 бит
                quint32 DisPoint = point.Dis;
                for (int i=0; i<32; ++i)
                {
                    if (DisPoint & 0x00000001)
                        TModel->AddDigitalPoint(tmpdv.at(i), 1);
                    else
                        TModel->AddDigitalPoint(tmpdv.at(i), 0);
                    DisPoint >>= 1;
                }
                for (int i=0; i<9; ++i)
                    TModel->AddAnalogPoint(tmpav.at(i), point.An[i]);
            }
            TModel->SetFilename(fn);
            dlg->setModal(false);
            dlg->PlotShow();
            dlg->show();
            break;
        }
        default:
            WARNMSG("ИД осциллограммы не определён");
            break;
        }
    }
    return Error::ER_NOERROR;
}*/
