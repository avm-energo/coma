#ifndef EOSCILLOGRAM_H
#define EOSCILLOGRAM_H

#include "trendviewmodel.h"

#include <QByteArray>
#include <QObject>
#include <QVector>
//#include "../dialogs/swjdialog.h"
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

#pragma pack(push) /* push current alignment to stack */
#pragma pack(1)    /* set alignment to 1 byte boundary */
    struct GBoStruct
    {
        quint32 FileNum;    // номер файла осциллограмм
        quint32 FileLength; // длина файла за исключением FileHeader (16 байт)
        quint32 ID; // Тип файла - осциллограмма и количество осциллограмм в файле (10000, 10001 ...)
        quint64 UnixTime; // Время начала записи осциллограммы
        quint32 IDo1; // ID первой осциллограммы в файле (определяет структуру точки и номер канала)
    };

    struct SWJournalRecordStruct
    {
        quint32 Num;      // Порядковый номер переключения
        quint8 NumA;      // Порядковый номер аппарата
        quint8 TypeA;     // Тип аппарата
        quint16 OpResult; // Результат операции: успешно / с неисправностью
        quint64 Time;     // Время, когда произведено переключение
        quint32 Options; // Направление переключения, тип коммутации и коммутируемые фазы
        float I[3];          // Значение тока в момент выдачи команды
        float U[3];          // Значение напряжения в момент выдачи команды
        quint16 OwnTime[3];  // Собственное время коммутации
        quint16 FullTime[3]; // Полное время коммутации (только для отключения, для включения будут нули)
        quint16 MovTime[3];  // Время перемещения главного контакта
        quint16 ArchTime[3]; // Время горения дуги
        quint16 IdleTime[3]; // Время безоперационного простоя
        quint16 Inaccuracy[3]; // Погрешность синхронной коммутации (только для соответствующего типа коммутации, для
                               // остальных типов нули
        float SupplyVoltage;   // Напряжение питания цепей соленоидов
        float Tokr;            // Температура окружающей среды
        float Tins[3];         // Температура внутри привода
        float Phyd[3];         // Давление в гидросистеме привода
        quint64 OscTime;       // Время старта осциллограммы
        quint8 Rezerv[4];      // Резерв
        quint32 timeF;         // Время записи в журнал
    };

    struct DataRecSwj
    {
        quint32 id; // 10031 – тип записи– переключения// 10031 – тип записи– переключения
        quint32 numbytes; // количество байт в записи переключения
        SWJournalRecordStruct SWJRec;
    };

    struct DataRecHeader
    {
        quint32 id;       // ID
        quint32 numbytes; // количество байт
    };

    /* struct OscHeader_Data
     {
         quint64 unixtime; // время первой точки в Unix-формате
         float step; // шаг по времени в мс
         quint32 len; // длина осциллограммы в количестве точек по времени
     };

     struct OscDataRec
     {
         quint32 id;
         quint32 numbytes;
     };*/

    // OscHeader_Data OscHeader;
    int BASize, BSize, Pos;
    QByteArray BA;
    SWJournalRecordStruct SWJRecord;
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

#pragma pack(pop) /* restore original alignment from stack */

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
