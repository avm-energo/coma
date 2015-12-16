#ifndef E_CONFDIALOG_H
#define E_CONFDIALOG_H

#include <QDialog>

#include "../widgets/s_tqcombobox.h"
#include "../widgets/s_tqcheckbox.h"
#include "../widgets/s_tqSpinBox.h"
#include "../publicclass.h"
#include "e_config.h"

// Подтипы модуля Э
#define ET_2T0N         0
#define ET_1T1N         1
#define ET_0T2N         2

#define ECONFER(a)       ERMSG(publicclass::ER_ECONF,__LINE__,a)
#define ECONFDBG         DBGMSG(publicclass::ER_ECONF,__LINE__)
#define ECONFWARN        WARNMSG(publicclass::ER_ECONF,__LINE__)
#define ECONFINFO(a)     INFOMSG(publicclass::ER_ECONF,__LINE__,a)

class e_confdialog : public QDialog
{
    Q_OBJECT
public:
    explicit e_confdialog(QWidget *parent = 0);

private:

    QByteArray confba;
    e_config *econf;

    void FillConfData();

signals:
    void BsiIsNeedToBeAcquiredAndChecked();

public slots:

private slots:
    void SetOsc(int, s_tqCheckBox *);
    void SetThreshold(double, s_tqSpinBox *);
    void SetCurrent(double, s_tqSpinBox *);
    void SetSecCurrent(int tmpi, s_tqComboBox *);
    void SetEqType(int);
    void SetVoltageClass(int, s_tqComboBox *);
    void Set104(double, s_tqSpinBox *);
    void SetCType(int);
    void SetNPoints(QString);
    void SetNFiltr(int);
    void SetNHFiltr(int);
    void GetBci();
    void WriteConfDataToModule();
    void SetupUI();
    void SetDefConf();
    void SetNewConf();
    void LoadConf();
    void SaveConf();
};

#endif // E_CONFDIALOG_H
