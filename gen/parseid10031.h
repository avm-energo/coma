#ifndef PARSEID10031_H
#define PARSEID10031_H

#include "parsemodule.h"

class ParseID10031 : public ParseModule
{
public:
    ParseID10031(QByteArray &BA);

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


    int Parse(int &count);
};

#endif // PARSEID10031_H
