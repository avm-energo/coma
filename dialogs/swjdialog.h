#ifndef SWJDIALOG_H
#define SWJDIALOG_H

#include <QDialog>
#include "../gen/eoscillogram.h"
#include "../dialogs/trendviewdialog.h"

class SWJDialog : public QDialog
{
    Q_OBJECT
public:
    enum Modes
    {
        SWJ_MODE_OFFLINE,
        SWJ_MODE_ONLINE
    };

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
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

    struct SWJINFStruct
    {
        quint16 FileNum;        // Номер файла
        quint32 FileLength;     // Размер файла
        quint32 Num;        	// Порядковый номер переключения
        quint8 	NumA;           // Порядковый номер аппарата
        quint8 	TypeA;          // Тип аппарата
        quint64 Time;           // Время, когда произведено переключение
        quint32 Options;        // Направление переключения, тип коммутации и коммутируемые фазы
    };


#pragma pack(pop)   /* restore original alignment from stack */




    SWJINFStruct SWJInf;
    //EOscillogram::GBoStruct GBOs;
    EOscillogram::DataRecSwj SwRec;
    EOscillogram *SWJOscFunc;
    bool Mode;
    TrendViewDialog *dlg;
    TrendViewModel *mdl;

    SWJDialog(EOscillogram *osc, int mode=SWJ_MODE_ONLINE, QWidget *parent = nullptr);
    void Init(SWJDialog::SWJINFStruct swj);
    void LoadOsc(QByteArray &ba); // для оффлайн режима
    void GetSwjOscData();

public slots:
#if PROGSIZE != PROGSIZE_EMUL
    void SaveSWJ();
    void ShowOsc();
#endif
};

#endif // SWJDIALOG_H
