#ifndef ABSTRACTALARM_H
#define ABSTRACTALARM_H


#include <QDialog>
#include <QByteArray>
#include "../iec104/iec104.h"
#include "../iec104/ethernet.h"
#include "../modbus/modbus.h"
#include "../gen/modulebsi.h"



class AbstractAlarm : public QWidget
{
        Q_OBJECT
public:
    AbstractAlarm(QWidget *parent = nullptr);

    const quint32 PredBSIMask = 0x00005F55;
    const quint32 AvarBSIMask = 0x000020AA;
\
   // QList <bool> WarnAlarmEvents;
  //  QList <bool> AvarAlarmEvents;

    quint8 PredAlarmEvents[20];
    quint8 AvarAlarmEvents[20];

    quint8 PredAlarmEventsKTF[15];
    quint8 AvarAlarmEventsKTF[15];

    struct Bd11
    {
        quint32 dev;
        quint32 predAlarm;
        quint32 alarm;
    };

    struct Bd16
    {
        quint32 Prib;
        quint32 Warn;
        quint32 Alarm;
    };


    static QStringList HthToolTip()
    {
        QStringList sl;
        //sl.append("Что-то не в порядке");
        sl.append("Проблемы со встроенным АЦП ");
        sl.append("Не работает внешняя flash-память");
        sl.append("Перегрев");
        sl.append("Проблемы с АЦП (нет связи) (базовая)");
        sl.append("Нет сигнала 1PPS с антенны");
        sl.append("Проблемы с АЦП (нет связи) (мезонин)");
        sl.append("Ошибка регулировочных коэффициентов (базовая)");
        sl.append("Ошибка загрузки конфигурации из flash-памяти. Работает конфигурация по умолчанию");
        sl.append("Некорректная Hardware информация (базовая)");
        sl.append("Некорректная Hardware информация (мезонин)");
        sl.append("Ошибка регулировочных коэффициентов (мезонин)");
        sl.append("Напряжение батареи низко (< 2,5 В)");
        sl.append("Нет связи с NTP-сервером");
        sl.append("Не работает внешняя flash-память (мезонин)");
        sl.append("Не работает внешняя fram");
        return sl;
    }


public slots:

  //  virtual void Update(QList<bool> values)=0;


    virtual void AlarmState()=0;
    virtual  void PredAlarmState()=0;
    virtual void AvarState()=0;

    virtual void UpdateUSB()=0;
    virtual void USBSetAlarms()=0;

     virtual void UpdatePredAlarmEvents(IEC104Thread::SponSignals *)=0;
     virtual void UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *)=0;

     virtual void ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)=0;
     virtual void ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)=0;

private:

};

#endif // ABSTRACTALARM_H
