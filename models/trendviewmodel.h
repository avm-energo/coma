#ifndef TRENDVIEWMODEL_H
#define TRENDVIEWMODEL_H

#define MT_ID85         10030 // ID осциллограммы по модулю 8085

#include <QVector>
#include <QMap>
#include "QtXlsx/xlsxdocument.h"
#include "../gen/colors.h"

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
    float xmax, xmin;
    quint32 Len;


    QStringList acolors_85 = QStringList() << YLWCOLOR << GRNCOLOR << REDCOLOR << \
                                           YLLCOLOR << GRLCOLOR << RDLCOLOR << \
                                           YLDCOLOR << GRDCOLOR << RDDCOLOR;
    QStringList dcolors_85 = QStringList() << YLWCOLOR << GRNCOLOR << REDCOLOR << \
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

    QStringList tmpdv_85;/* = QStringList() << "OCNA" << "OCNB" << "OCNC" << "OCFA" << "OCFB" << "OCFC" \
                                         << "BKCA" << "BKCB" << "BKCC" << "BKOA" << "BKOB" << "BKOC" \
                                         << "CSC" << "CSO" << "CNA" << "CNB" << "CNC" << "CFA" << "CFB" << "CFC" \
                                         << "nNA" << "nNB" << "nNC" << "nFA" << "nFB" << "nFC" << "nCA" << "nCB" << "nCC" \
                                         << "nOA" << "nOB" << "nOC";*/
    QStringList tmpav_85;// = QStringList() << "USA" << "USB" << "USC" << "IA" << "IB" << "IC" << "ULA" << "ULB" << "ULC";

    QStringList acolors_80 = QStringList() << YLWCOLOR << GRNCOLOR << REDCOLOR << \
                                           YLLCOLOR << GRLCOLOR << RDLCOLOR << \
                                           YLDCOLOR << GRDCOLOR << RDDCOLOR;
    QStringList dcolors_80 = QStringList() << YLWCOLOR << GRNCOLOR << REDCOLOR << \
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

    QStringList tmpdv_80 = QStringList();
    QStringList tmpav_80 = QStringList() << "USA" << "USB" << "USC" << "IA" << "IB" << "IC" << "ULA" << "ULB" << "ULC";


    QStringList tmpav_21 = QStringList() << "UA" << "UB" << "UC" << "IA" << "IB" << "IC";

private:


    int PointsNum;
    QString Filename;
    bool NoDiscrete, NoAnalog;
    QStringList DigitalNames, AnalogNames;
    int WRow;



    void WriteToFile(int row, QXlsx::Document *xls);

public slots:
    void SaveToExcel(QWidget *parent);
    void SaveToComtrade();
};

#endif // TRENDVIEWMODEL_H
