#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QMap>
#include "../config/config.h"
#include "../iec104/iec104.h"

class InfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InfoDialog(QWidget *parent = nullptr);

    typedef struct
    {
       quint32 Val;
       int SigAdr;
    }ModBusBSISignal;

    void SetupUI();

signals:

public slots:
    void FillBsi();
    void ClearBsi();
    void FillBsiFrom104(Parse104::BS104Signals* BS104);
    void FillBsiFromModBus(ModBusBSISignal *Signal, int * size);

private:
/*    QMap<int, QString> ModuleNames()
    {
        QMap<int, QString> map;
        map[MTB_A1] = "ПКДН";
        map[MTB_80] = "АВТУК";
        return map;
    } */
};

#endif // INFODIALOG_H
