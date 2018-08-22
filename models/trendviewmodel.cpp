#include <QVector>
#include <QPen>
#include <QAction>
#include <QFileDialog>
#include "QtXlsx/xlsxdocument.h"
#include "../config/config.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "trendviewmodel.h"
#include "../widgets/wd_func.h"

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

void TrendViewModel::SaveToExcel(QWidget *parent)
{
    QXlsx::Document *xlsx;
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString Filename = dlg->getSaveFileName(parent, "Сохранить данные",StdFunc::GetHomeDir(),"Excel files (*.xlsx)", \
                                            nullptr, QFileDialog::DontUseNativeDialog);
    xlsx = new QXlsx::Document(Filename);
    QStringList sl = Filename.split("#"); // отделяем имя файла от даты-времени
    Filename = sl.at(0);
    QString OscDateTime = sl.at(1);
    xlsx->write(1,1,QVariant("Модуль: 85"));
    xlsx->write(2,1,QVariant("Дата: "+OscDateTime.split(" ").at(0)));
    xlsx->write(3,1,QVariant("Время: "+OscDateTime.split(" ").at(1)));
    xlsx->write(4,1,QVariant("Смещение, мс"));

    WriteToFile(5, xlsx);
    //xlsx->save();
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

void TrendViewModel::WriteToFile(int row, QXlsx::Document *xls)
{
    QXlsx::Format format;
    QString Precision = "0.0000";
    format.setNumberFormat(Precision);

     quint32 Digitalpointscurkey;
     float Analogpointscurkey;

     int col = 2;// 2 = OCNA
     for (int j=0; j<14; ++j)  //j<tmpdv->size() используем только 14 бит
     {
         QVector<double> tmpv = DigitalMainData.value(tmpdv[j]);

         for (int i=0; i<tmpv.size(); ++i)
         {
           Digitalpointscurkey = static_cast<quint32>(tmpv.at(i));

           if (Digitalpointscurkey & 0x00000001)
               xls->write((row+i), col, QVariant("1"));
           else
               xls->write((row+i), col, QVariant("0"));

         }

         col++;
     }
     col = 16;

     for (int j=0; j<tmpav->size(); ++j)
     {
         QVector<double> tmpa = AnalogMainData.value(tmpav[j]);

         for (int i=0; i<tmpa.size(); ++i)
         {
             Analogpointscurkey = static_cast<float>(tmpa.at(i));

             xls->write((row+i), col, QVariant(QString::number(Analogpointscurkey, 'f', 6)));

         }

         col++;
     }




       /* for (int i=0; i<3; i++)
        {
            xls->write(row,i+2, QVariant(QString::number(AnalogMainData.value(&"USA"[40*i]))));
            xls->write(row,i+5,WDFunc::FloatValueWithCheck(Bd_block1.IUefNat_filt[i+3]), format);
            xls->write(row,i+8,WDFunc::FloatValueWithCheck(Bd_block1.IUefNat_filt[i+6]), format);

        }
        xls->write(row,23,Bd_block1.Frequency,format);

        AnalogMainData.values();

        row = 5;
        for (int i = 0; i < MainPoints.size(); ++i) // цикл по точкам
        {
            int col = 2; // 2 = OCNA
            for (int i=0; i<14; ++i)
            {
                if (DigitalMainData.value("OCNA") & 0x00000001)
                    xls->write(row, col++, QVariant("1"));
                else
                    xls->write(row, col++, QVariant("0"));
                point.Dis >>= 1;
            }
            col = 16;

            while (col < 31)
            {
            xls->write(row, col, QVariant(QString::number(point.An[col-16], 'f', 6)));
            col++;
            }
       }*/

}
