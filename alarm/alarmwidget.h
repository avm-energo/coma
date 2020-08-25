#ifndef ALARMWIDGET_H
#define ALARMWIDGET_H

#include "../alarm/alarmclass.h"
#include "../gen/colors.h"
#include "../gen/commands.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"
#include "../widgets/wd_func.h"

#include <QByteArray>
#include <QDialog>
#include <QMap>

class AlarmWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmWidget(AlarmClass *alarm, QWidget *parent = nullptr);

signals:
    void SetWarnAlarmColor(QList<bool>);
    void SetAlarmColor(QList<bool>);
    void AlarmButtonPressed();
    void ModuleWarnButtonPressed();
    void ModuleAlarmButtonPressed();

public slots:

    void UpdateFirstUSB();
    void UpdateSecondUSB(QList<bool>);
    void UpdateThirdUSB(QList<bool>);

private:
    AlarmClass *Alarm;
};

#endif // ALARMWIDGET_H
