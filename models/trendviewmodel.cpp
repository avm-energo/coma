#include <QVector>
#include <QPen>
#include <QAction>
#include "QtXlsx/xlsxdocument.h"
#include "../config/config.h"
#include "../gen/error.h"
#include "trendviewmodel.h"

TrendViewModel::TrendViewModel(const QStringList &dlist, const QStringList &alist, \
                                 int pointsnum)

{
    this->PointsNum = pointsnum;
    DigitalNames = dlist;
    AnalogNames = alist;
    for (int i=0; i<dlist.size(); ++i)
        DigitalMainData[dlist.at(i)] = QVector<double>();
    for (int i=0; i<alist.size(); ++i)
        AnalogMainData[alist.at(i)] = QVector<double>();
}

TrendViewModel::~TrendViewModel()
{

}

void TrendViewModel::AddAnalogPoint(const QString &GraphNum, float PointValue)
{
    if (AnalogMainData.contains(GraphNum))
    {
        QVector<double> tmpv = AnalogMainData[GraphNum];
        tmpv.append(PointValue);
        AnalogMainData[GraphNum] = tmpv;
    }
}

void TrendViewModel::AddDigitalPoint(const QString &GraphNum, int PointValue)
{
    if (DigitalMainData.contains(GraphNum))
    {
        QVector<double> tmpv = DigitalMainData[GraphNum];
        double value = static_cast<double>(PointValue) + static_cast<double>(DigitalNames.indexOf(GraphNum)) * 1.5f;
        tmpv.append(value);
        DigitalMainData[GraphNum] = tmpv;
    }
}

void TrendViewModel::SaveToExcel()
{
    QStringList sl;
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
}

void TrendViewModel::SaveToComtrade()
{

}

bool TrendViewModel::SetPointsAxis(float start, float step)
{
    if (step <= 0)
    {
        WARNMSG("Ошибка в принятой осциллограмме, шаг отрицательный или равен нулю");
        return false;
    }
    if (PointsNum == 0)
        return false;
    MainPoints.clear();
    for (int i=0; i<PointsNum; ++i)
    {
        MainPoints << start;
        start += step;
    }
    return true;
}

void TrendViewModel::SetFilename(const QString &fn)
{
    Filename = fn;
}

int TrendViewModel::DContains(const QString &key)
{
    return DigitalMainData.keys().contains(key);
}

int TrendViewModel::AContains(const QString &key)
{
    return AnalogMainData.keys().contains(key);
}
