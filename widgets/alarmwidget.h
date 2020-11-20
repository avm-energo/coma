#ifndef ALARMWIDGET_H
#define ALARMWIDGET_H

//#include "../alarm/alarmclass.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/uwidget.h"
#include "../widgets/wd_func.h"

#include <QByteArray>
#include <QDialog>

#define BSIWARNMASK 0x00005F55
#define BSIALARMMASK 0x000020AA

class AlarmWidget : public UWidget
{
    Q_OBJECT
public:
    explicit AlarmWidget(QWidget *parent = nullptr);
    void Clear();
    void uponInterfaceSetting();

signals:
    //    void SetWarnAlarmColor(QList<bool>);
    //    void SetAlarmColor(QList<bool>);
    void AlarmButtonPressed();
    void ModuleWarnButtonPressed();
    void ModuleAlarmButtonPressed();

public slots:

    //    void UpdateFirstUSB();
    //    void UpdateSecondUSB(QList<bool>);
    //    void UpdateThirdUSB(QList<bool>);
    //    void UpdateSecondUSB(bool);
    //    void UpdateThirdUSB(bool);
    void UpdateIndicator(bool);
    //    void update(bool w, bool a);
    void updateWarn(bool isset);
    void updateAlarm(bool isset);
    void updateMain(bool isset);

private:
    //    AlarmClass *Alarm;
};

#endif // ALARMWIDGET_H
