#ifndef ALARMSTATEALL_H
#define ALARMSTATEALL_H

#define CIRCLE_RADIUS 15
#define BSI_HEALTH_MASK 0xFFFF
#include <QWidget>

class AlarmStateAll : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmStateAll(QWidget *parent = nullptr);

public slots:

    //    void AlarmState();
    void UpdateHealth(quint32 health);
    //    void CallUpdateHealth();

    void update();

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

signals:
    void BSIUpdated(bool isset);

private:
    const QList<int> WarnPositions = { 1, 3, 5, 7, 13 };
};

#endif // ALARMSTATE_H
