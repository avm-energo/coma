#ifndef CONFDIALOG31_H
#define CONFDIALOG31_H

#include <QDialog>

#include "abstractconfdialog3x.h"
#include "config31.h"

// Описание блока Bci
#define DBCI_INTYPE     11
#define DBCI_DLY        12
#define DBCI_PAIR       16
#define DCONF_NUM       17

class ConfDialog31 : public AbstractConfDialog3x
{
    Q_OBJECT
public:
    explicit ConfDialog31(QVector<publicclass::DataRec> &S2Config, bool BaseBoard=true, QWidget *parent = 0);

private:
    publicclass::DataRec Config[DCONF_NUM];
    Config31 *C31;

    void Fill();
    void SetupUI();
    void SetPair(int firstch, int secondch);

signals:
    void BsiIsNeedToBeAcquiredAndChecked();

public slots:

private slots:
/*    void SetChTypData(int, s_tqComboBox *);
    void SetChOsc(int, s_tqCheckBox *);
    void SetChOscSrc(int, s_tqComboBox *);
    void SetInMin(double, s_tqSpinBox *);
    void SetInMax(double, s_tqSpinBox *);
    void SetInVMin(double, s_tqSpinBox *);
    void SetInVMax(double, s_tqSpinBox *);
    void SetMinMin(double, s_tqSpinBox *);
    void SetMin(double, s_tqSpinBox *);
    void SetMax(double, s_tqSpinBox *);
    void SetMaxMax(double, s_tqSpinBox *);
    void Set104(double, s_tqSpinBox *);
    void SetCType(int);
    void SetOscDly(int);
    void GetBci();
    void WriteConfDataToModule();
    void SetNewConf();
    void SetDefConf(); */
};

#endif // CONFDIALOG31_H
