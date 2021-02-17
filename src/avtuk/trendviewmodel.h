#pragma once

#include "../gen/colors.h"

#include <QMap>
#include <QVector>

// constexpr int MT_ID85 = 10030; // ID осциллограммы по модулю 8085
class TrendViewModel
{
public:
    TrendViewModel(const QStringList &dlist, const QStringList &alist, int pointsnum);
    ~TrendViewModel();

    QMap<QString, QVector<double>> AnalogMainData, DigitalMainData;
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

    QStringList acolors_85 {
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR  //
    };
    QStringList dcolors_85 {
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR, //
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::BLUCOLOR, //
        Colors::BLDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR  //
    };
    QStringList ddescr_85 {
        "Оперативное управление включением полюса А",                                                 //
        "Оперативное управление включением полюса B",                                                 //
        "Оперативное управление включением полюса C",                                                 //
        "Оперативное управление отключением полюса А",                                                //
        "Оперативное управление отключением полюса B",                                                //
        "Оперативное управление отключением полюса C",                                                //
        "НЗ блок-контакт полюса А", "НЗ блок-контакт полюса B",                                       //
        "НЗ блок-контакт полюса C", "НО блок-контакт полюса А",                                       //
        "НО блок-контакт полюса В", "НО блок-контакт полюса С", "Контроль цепи соленоида отключения", //
        "Контроль цепи соленоида включения", "Выходная команда на включение полюса А",                //
        "Выходная команда на включение полюса В", "Выходная команда на включение полюса С",           //
        "Выходная команда на отключение полюса А", "Выходная команда на отключение полюса В",         //
        "Выходная команда на отключение полюса С"                                                     //
    };

    QStringList adescr_85 {
        "Напряжение сети ф.А",        //
        "Напряжение сети ф.В",        //
        "Напряжение сети ф.С",        //
        "Ток в выключателе ф.А",      //
        "Ток в выключателе ф.В",      //
        "Ток в выключателе ф.С",      //
        "Напряжение на нагрузке ф.А", //
        "Напряжение на нагрузке ф.В", //
        "Напряжение на нагрузке ф.С"  //
    };
    QStringList
        tmpdv_85; /* = QStringList() << "OCNA" << "OCNB" << "OCNC" << "OCFA" << "OCFB" << "OCFC" \
                                  << "BKCA" << "BKCB" << "BKCC" << "BKOA" << "BKOB" << "BKOC" \
                                  << "CSC" << "CSO" << "CNA" << "CNB" << "CNC" << "CFA" << "CFB" << "CFC" \
                                  << "nNA" << "nNB" << "nNC" << "nFA" << "nFB" << "nFC" << "nCA" << "nCB" << "nCC" \
                                  << "nOA" << "nOB" << "nOC";*/
    QStringList
        tmpav_85; // = QStringList() << "USA" << "USB" << "USC" << "IA" << "IB" << "IC" << "ULA" << "ULB" << "ULC";

    QStringList acolors_80 { Colors::YLWCOLOR, Colors::GRNCOLOR, Colors::REDCOLOR, Colors::YLLCOLOR, Colors::GRLCOLOR,
        Colors::RDLCOLOR, Colors::YLDCOLOR, Colors::GRDCOLOR, Colors::RDDCOLOR };
    QStringList dcolors_80 { Colors::YLWCOLOR, Colors::GRNCOLOR, Colors::REDCOLOR, Colors::YLLCOLOR, Colors::GRLCOLOR,
        Colors::RDLCOLOR, Colors::YLDCOLOR, Colors::GRDCOLOR, Colors::RDDCOLOR, Colors::YLWCOLOR, Colors::GRNCOLOR,
        Colors::REDCOLOR, Colors::BLUCOLOR, Colors::BLDCOLOR, Colors::YLLCOLOR, Colors::GRLCOLOR, Colors::RDLCOLOR,
        Colors::YLDCOLOR, Colors::GRDCOLOR, Colors::RDDCOLOR, Colors::YLWCOLOR, Colors::GRNCOLOR, Colors::REDCOLOR,
        Colors::YLLCOLOR, Colors::GRLCOLOR, Colors::RDLCOLOR, Colors::YLDCOLOR, Colors::GRDCOLOR, Colors::RDDCOLOR,
        Colors::YLWCOLOR, Colors::GRNCOLOR, Colors::REDCOLOR };

    QStringList tmpdv_80 = QStringList();
    QStringList tmpav_80 { "USA", "USB", "USC", "IA", "IB", "IC", "ULA", "ULB", "ULC" };

    QStringList tmpav_21 { "UA", "UB", "UC", "IA", "IB", "IC" };

private:
    int PointsNum;
    QString Filename;
    bool NoDiscrete, NoAnalog;
    QStringList DigitalNames, AnalogNames;
    int WRow;

    // void WriteToFile(int row, QXlsx::Document *xls);

    // public slots:
    // void SaveToExcel(QWidget *parent);
    // void SaveToComtrade();
};
