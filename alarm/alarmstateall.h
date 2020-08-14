#ifndef ALARMSTATEALL_H
#define ALARMSTATEALL_H

#include "../alarm/abstractalarm.h"

class AlarmStateAll : public AbstractAlarm
{
    Q_OBJECT
public:
    explicit AlarmStateAll(QWidget *parent = nullptr);

public slots:

    void AlarmState();
    void WarnAlarmState();
    void AvarState();

    void Update(QList<bool>);

    //   void UpdatePredAlarmEvents(IEC104Thread::SponSignals *);
    //   void UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *);

    //   void ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal);
    //   void ModBusUpdate(ModBus::Coils Signal);

    static QStringList HthToolTip()
    {
        QStringList sl;
        // sl.append("Что-то не в порядке");
        sl.append("Проблемы со встроенным АЦП ");
        sl.append("Не работает внешняя flash-память");
        sl.append("Перегрев");
        sl.append("Проблемы с АЦП (нет связи) (базовая)");
        sl.append("Нет сигнала 1PPS с антенны");
        sl.append("Проблемы с АЦП (нет связи) (мезонин)");
        sl.append("Ошибка регулировочных коэффициентов (базовая)");
        sl.append("Ошибка загрузки конфигурации из flash-памяти. Работает "
                  "конфигурация по умолчанию");
        sl.append("Некорректная Hardware информация (базовая)");
        sl.append("Некорректная Hardware информация (мезонин)");
        sl.append("Ошибка регулировочных коэффициентов (мезонин)");
        sl.append("Напряжение батареи низко (< 2,5 В)");
        sl.append("Нет связи с NTP-сервером");
        sl.append("Не работает внешняя flash-память (мезонин)");
        sl.append("Не работает внешняя fram");
        return sl;
    }

private:
};

#endif // ALARMSTATE_H
