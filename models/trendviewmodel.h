 #ifndef TRENDVIEWMODEL_H
#define TRENDVIEWMODEL_H

#define MT_ID85         10030 // ID осциллограммы по модулю 8085

#include <QVector>
#include <QMap>
#include "QtXlsx/xlsxdocument.h"

class TrendViewModel
{
public:
    TrendViewModel(const QStringList &dlist, const QStringList &alist, int pointsnum);
    ~TrendViewModel();

    QMap<QString, QVector<double> > AnalogMainData, DigitalMainData;
    QVector<double> MainPoints;

     // инициализация графиков
    // имена графиков контактных/аналоговых сигналов, количество точек, диапазон по оси Y для аналоговых
    void AddAnalogPoint(const QString &GraphNum, float PointValue);
    void AddDigitalPoint(const QString &GraphNum, int PointValue);
    bool SetPointsAxis(float start, float step);
    void SetFilename(const QString &fn);
    int DContains(const QString &key);
    int AContains(const QString &key);    
    static void SaveID(quint32 id);
    static quint32 idOsc;

private:


    int PointsNum;
    QString Filename;
    bool NoDiscrete, NoAnalog;
    QStringList DigitalNames, AnalogNames;
    int WRow;
    QString tmpdv[32] = {"OCNA","OCNB","OCNC","OCFA","OCFB","OCFC",
             "BKCA", "BKCB","BKCC","BKOA","BKOB","BKOC",
             "CSC","CSO","CNA","CNB","CNC","CFA","CFB","CFC",
             "nNA","nNB","nNC","nFA","nFB","nFC","nCA","nCB","nCC",
             "nOA","nOB","nOC"};
    QString tmpav[9]={ "USA","USB","USC","IA","IB","IC","ULA","ULB","ULC"};


    void WriteToFile(int row, QXlsx::Document *xls);

public slots:
    void SaveToExcel(QWidget *parent);
    void SaveToComtrade();
};

#endif // TRENDVIEWMODEL_H
