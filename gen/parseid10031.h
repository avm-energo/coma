#ifndef PARSEID10031_H
#define PARSEID10031_H

#include <QByteArray>
#include <QVector>
#include <QObject>
#include "parsemodule.h"
#include "../models/trendviewmodel.h"



class ParseID10031 : public ParseModule
{
public:
    ParseID10031(QByteArray &BA);

//    static quint32 len2;



    struct SWJournalRecordStruct
    {
        quint32 Num;        	// Порядковый номер переключения
        quint8 	NumA;           // Порядковый номер аппарата
        quint8 	TypeA;          // Тип аппарата
        quint16 OpResult;       // Результат операции: успешно / с неисправностью
        quint64 Time;           // Время, когда произведено переключение
        quint32 Options;        // Направление переключения, тип коммутации и коммутируемые фазы
        float	I[3];       	// Значение тока в момент выдачи команды
        float 	U[3];   		// Значение напряжения в момент выдачи команды
        quint16 OwnTime[3];		// Собственное время коммутации
        quint16 FullTime[3];	// Полное время коммутации (только для отключения, для включения будут нули)
        quint16 MovTime[3];		// Время перемещения главного контакта
        quint16 ArchTime[3];	// Время горения дуги
        quint16 IdleTime[3];	// Время безоперационного простоя
        quint16 Inaccuracy[3];	// Погрешность синхронной коммутации (только для соответствующего типа коммутации, для остальных типов нули
        float 	SupplyVoltage;	// Напряжение питания цепей соленоидов
        float 	Tokr;			// Температура окружающей среды
        float 	Tins[3];       	// Температура внутри привода
        float 	Phyd[3];		// Давление в гидросистеме привода
        quint64 OscTime;		// Время старта осциллограммы
        quint8 Rezerv[4];	    // Резерв
        quint32 timeF;		    // Время записи в журнал
    };

    struct OscHeader_Data
    {
        quint64 unixtime; // время первой точки в Unix-формате
        float step; // шаг по времени в мс
        quint32 len; // длина осциллограммы в количестве точек по времени
    };

    struct Point85
    {
        float An[9];    // Ua,Ub,Uc (напряжение источника), Ia, Ib, Ic (ток ВВ), Ua,Ub,Uc (напряжение нагрузки)
        quint32 Dis;
    };

    int Parse(int &count);
//    void Save(quint32 *len);

     int ParseID85(OscHeader_Data &OHD, const QString &fn, int &count);

private:
//     quint32 Len;
};

#endif // PARSEID10031_H
