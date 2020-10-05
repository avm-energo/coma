#ifndef ALARMWIDGET_H
#define ALARMWIDGET_H

#include "../alarm/alarmclass.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"

#include <QByteArray>
#include <QDialog>
#include <QMap>

#define BSIWARNMASK 0x00005F55
#define BSIALARMMASK 0x000020AA

class AlarmWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmWidget(AlarmClass *alarm, QWidget *parent = nullptr);
    void Clear();

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
    void UpdateIndicator(bool);

private:
    AlarmClass *Alarm;
};

#endif // ALARMWIDGET_H
