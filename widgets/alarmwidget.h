#ifndef ALARMWIDGET_H
#define ALARMWIDGET_H

#include "../module/basealarm.h"
#include "../widgets/uwidget.h"
//#define BSIWARNMASK 0x00005F55
//#define BSIALARMMASK 0x000020AA

class AlarmWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmWidget(QWidget *parent = nullptr);
    void clear();
    // void uponInterfaceSetting();

    void addAlarm(BaseAlarm *alarm);

signals:
    //    void SetWarnAlarmColor(QList<bool>);
    //    void SetAlarmColor(QList<bool>);
    //    void AlarmButtonPressed();
    //    void ModuleWarnButtonPressed();
    //    void ModuleAlarmButtonPressed();

public slots:

    //    void UpdateFirstUSB();
    //    void UpdateSecondUSB(QList<bool>);
    //    void UpdateThirdUSB(QList<bool>);
    //    void UpdateSecondUSB(bool);
    //    void UpdateThirdUSB(bool);
    void UpdateIndicator(bool);
    //    void update(bool w, bool a);
    //    void updateWarn(bool isset);
    //    void updateAlarm(bool isset);
    //    void updateMain(bool isset);
    //    void updateMain(QColor color);
    //    void updateWarn(QColor color);
    //    void updateAlarm(QColor color);
    //    void buttonClicked(QWidget *button);

private:
    // QList<BaseAlarm *> m_alarms;
    // QDialogButtonBox *buttons;
    QTimer *m_timer;
    const QStringList buttonDescription {
        "Состояние устройства",           //
        "Предупредительная сигнализация", //
        "Аварийная сигнализация"          //
    };
    //    AlarmClass *Alarm;
};

#endif // ALARMWIDGET_H
