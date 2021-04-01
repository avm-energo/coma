#ifndef CONFDIALOGKDV_H
#define CONFDIALOGKDV_H

#include "abstractconfdialog.h"
class ConfigKDV;
class ConfKDVDialog : public AbstractConfDialog
{
public:
    explicit ConfKDVDialog(ConfigKDV *ckdv, QWidget *parent = nullptr);
    ~ConfKDVDialog();

private:
    //    void Fill() override;
    //    void FillBack() const override;
    //    void SetupUI() override;
    void CheckConf() const override;

protected:
    //    QWidget *analogWidget();
    //    QWidget *settingWidget();
    //    QWidget *alarmWidget();
    //    QWidget *commonAlarmWidget();
    //    QWidget *vibrAlarmWidget();
    //    QWidget *hystWidget();
    //    QWidget *otherWidget();
    //    QWidget *recordWidget();
    //    QWidget *motorWidget();
    //    virtual QWidget *connectionWidget();

    //    void FillKdv();
    //    void FillBackKdv() const;

    ConfigKDV *ConfKDV;

private:
    void SetDefConf() override;
};

#endif // CONFIGDIALOGKDV_H
