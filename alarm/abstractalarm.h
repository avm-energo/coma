#ifndef ABSTRACTALARM_H
#define ABSTRACTALARM_H


#include <QDialog>
#include <QByteArray>
#include "../iec104/iec104.h"
#include "../iec104/ethernet.h"
#include "../modbus/modbus.h"
#include "../check/eabstractcheckdialog.h"

class AbstractAlarm : public QDialog
{
        Q_OBJECT
public:
    AbstractAlarm(QWidget *parent = nullptr);


    virtual void AlmNewTimers()=0;



    struct Bd11
    {
        quint32 dev;
        quint32 predAlarm;
        quint32 alarm;
    };


    const quint32 PredBSIMask = 0x00005F55;
    const quint32 AvarBSIMask = 0x000020AA;

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


    QWidget *AlmReleWidget();


 signals:
      void SetPredAlarmColor(quint8*);
      void SetAlarmColor(quint8*);


public slots:
     virtual void UpdatePredAlarmEvents(IEC104Thread::SponSignals *)=0;
     virtual void UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *)=0;
     virtual void DeviceState()=0;
     virtual  void PredAlarmState()=0;
     virtual void AlarmState()=0;
     virtual void UpdateUSB()=0;
     virtual void USBSetAlarms()=0;
     virtual void ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)=0;
     virtual void ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)=0;

private:
     // QTimer* BdaTimer, *TimeTimer, *AlarmStateTimer;

       //virtual void AlmNewTimers()=0;
 //      EAbstractCheckDialog *CheckB, *CheckM;

};

#endif // ABSTRACTALARM_H
