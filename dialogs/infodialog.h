#ifndef INFODIALOG_H
#define INFODIALOG_H

#include "../config/config.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"

#include <QDialog>
#include <QMap>

class InfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InfoDialog(QWidget *parent = nullptr);

    void SetupUI();
    void FillBsi();

signals:

public slots:
    void ClearBsi();
    //    void FillBsiFrom104(Parse104::BS104Signals* BS104);
    //    void FillBsiFromModBus(QList<ModBus::BSISignalStruct> Signal, int size);

private:
};

#endif // INFODIALOG_H
