#ifndef CONFDIALOGKDV_H
#define CONFDIALOGKDV_H

#include "../config/configkdv.h"
#include "abstractconfdialog.h"

class ConfKDVDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfKDVDialog(ConfigKDV *ckdv, QWidget *parent = nullptr);
    ~ConfKDVDialog();

private:
    bool Variable;

    void Fill() override;
    void FillBack() override;
    void SetupUI() override;
    void CheckConf() override;

protected:
    QWidget *analogWidget();
    QWidget *setWidget();
    QWidget *alarmWidget();
    QWidget *otherWidget();
    virtual QWidget *connectionWidget();

    void FillKdv();
    void FillBackKdv();

    ConfigKDV *ConfKDV;

private slots:

    void SetDefConf() override;
};

#endif // CONFIGDIALOGKDV_H
