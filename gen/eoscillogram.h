#ifndef EOSCILLOGRAM_H
#define EOSCILLOGRAM_H

#include "../qcustomplot.h"

#define MT_HEAD_ID      9000 // ID осциллограммы
#define MT_ID21         10001 // первый ID осциллограммы по модулю 21
#define MT_ID21E        10016 // последний ID осциллограммы по модулю 21
#define MT_ID81         10021 // ID осциллограммы по модулю 8081
#define MT_ID82         10022 // ID осциллограммы по модулю 8082
#define MT_ID83         10023 // ID осциллограммы по модулю 8083
#define MT_ID85         10030 // ID осциллограммы по модулю 8085

class EOscillogram : public QObject
{
    Q_OBJECT
public:
    EOscillogram(QObject *parent=0);
    ~EOscillogram();

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

    struct OscHeader_Data
    {
        quint64 unixtime; // время первой точки в Unix-формате
        float step; // шаг по времени в мс
        quint32 len; // длина осциллограммы в количестве точек по времени
    };

    struct OscDataRec
    {
        quint32 id;
        quint32 numbytes;
    };

    OscHeader_Data OscHeader;
    int BASize, BSize, Pos;
    QByteArray BA;

    struct Point85
    {
        quint32 Dis;
        float An[9];    // Ua,Ub,Uc (напряжение источника), Ia, Ib, Ic (ток ВВ), Ua,Ub,Uc (напряжение нагрузки)
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
        float U1[3];
        float U2[3];
    };

#pragma pack(pop)   /* restore original alignment from stack */

    void AddAnalogPoint(int GraphNum, float PointValue);
    void AddDigitalPoint(int GraphNum, int PointValue);
    void SetFilename(const QString &fn);
    bool PosPlusPlus(void *dst, int size);
//    int ProcessOsc(quint32 oscid, quint32 oscnum, const QString &OscDateTime); // сама осциллограмма в BA
    int ProcessOsc();
    int ProcessOneOsc(quint32 id, OscHeader_Data &OHD, const QString &fn); // id - ИД осциллограммы (по табл. Приложения 2 "АВ-ТУК. Файлы"), осциллограмма в BA[Pos]

private:
    QVector<QVector<double> > AnalogMainData, DigitalMainData;
    QVector<double> MainPoints;
    int PointsNum;
    QString Filename;

public slots:

private slots:
    void SaveToExcel();
    void SaveToComtrade();
};

#endif // EOSCILLOGRAM_H
