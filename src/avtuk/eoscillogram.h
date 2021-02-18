#ifndef EOSCILLOGRAM_H
#define EOSCILLOGRAM_H

#include "../gen/datatypes.h"
#include "trendviewmodel.h"

#include <QByteArray>
#include <QObject>
#include <QVector>
constexpr int MT_HEAD_ID = 9000; // ID осциллограммы
constexpr int MT_HEAD87 = 9050;  // ID осциллограммы и спектрограммы модуля 87
constexpr int SWJ_ID85 = 10031;  // ID осциллограммы c журналом по модулю 8085

constexpr int MAXOSCBUFSIZE = 262144; // максимальный размер буфера для осциллограмм

class EOscillogram : public QObject
{
    Q_OBJECT
public:
    EOscillogram(QObject *parent = nullptr);
    ~EOscillogram();

    // OscHeader_Data OscHeader;
    int BASize, BSize, Pos;
    QByteArray BA;
    S2DataTypes::SwitchJourRecord SWJRecord;
    // TrendViewModel mdl;
    /* struct Point85
     {
         float An[9];    // Ua,Ub,Uc (напряжение источника), Ia, Ib, Ic (ток ВВ), Ua,Ub,Uc (напряжение нагрузки)
         quint32 Dis;
     };
                         // Dis:
                         // 31-29:	nOx – НО блок-контакты полюсов А, В, С (нефильтр.)
                         // 28-26:	nCx – НЗ блок-контакты полюсов А, В, С (нефильтр.)
                         // 25-23:	nFx – оперативное управление отключением полюсов А, В, С (нефильтр.)
                         // 22-20:	nNx – оперативное управление включением полюсов А, В, С (нефильтр.)
                         // 19-17:	CFx – выходные команды на отключение полюсов А, В, С
                         // 16-14:	CNx – выходные команды на включение полюсов А, В, С
                         // 13:      CSC – контроль цепи соленоида отключения
                         // 12:      CSO – контроль цепи соленоида включения
                         // 11-9:	BKOx – НО блок-контакты полюсов А, В, С
                         // 8-6:     BKCx – НЗ блок-контакты полюсов А, В, С
                         // 5-3:     OCFx – оперативное управление отключением полюсов А, В, С
                         // 2-0:     OCNx – оперативное управление включением полюсов А, В, С

     struct Point21
     {
         float An;
     };

     struct Point8x
     {
         float An[6];
     };*/

    //#pragma pack(pop) /* restore original alignment from stack */

    /*    void AddAnalogPoint(int GraphNum, float PointValue);
        void AddDigitalPoint(int GraphNum, int PointValue);
        void SetFilename(const QString &fn); */
    bool PosPlusPlus(void *dst, int size);
    //    int ProcessOsc(quint32 oscid, quint32 oscnum, const QString &OscDateTime); // сама осциллограмма в BA
    bool ProcessOsc(TrendViewModel *mdl);
    // int ProcessOneOsc(quint32 id, OscHeader_Data &OHD, const QString &fn); // id - ИД осциллограммы (по табл.
    // Приложения 2 "АВ-ТУК. Файлы"), осциллограмма в BA[Pos]

private:
    QVector<QVector<double>> AnalogMainData, DigitalMainData;
    QVector<double> MainPoints;
    int PointsNum;
    QString Filename;

public slots:

private slots:
    /*    void SaveToExcel();
        void SaveToComtrade(); */
};

#endif // EOSCILLOGRAM_H
