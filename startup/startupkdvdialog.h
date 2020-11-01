#ifndef CORDIALOGKDV_H
#define CORDIALOGKDV_H

//#include "../iec104/iec104.h"
//#include "../modbus/modbus.h"
#include "../models/etablemodel.h"
#include "abstractstartupdialog.h"

#include <QByteArray>
#include <QDialog>

class StartupKDVDialog : public AbstractStartupDialog
{
    Q_OBJECT
public:
    StartupKDVDialog(QWidget *parent = nullptr);
    ~StartupKDVDialog();

    // int corDIndex;
    // int first;

    void GetCorBd() override;

private:
    struct WBd7
    {
        float InitAge;
    };

    struct WBd8
    {
        float MHnInit;
        float MotHovInit;
    };
    struct Bd9
    {
        float Vst;
        float Age;
        float Resurs;
        float MotHnorm;
        float MotHover;
        float MotHTotal;
        float reserv;
    };

    Bd9 *Bd9Block;
    WBd7 *WBd7Block;
    WBd8 *WBd8Block;

    void FillBackCor() override;
    void FillCor() override;
    void FillBackWBd8();

    //    float ToFloat(QString text);
    void SetupUI() override;
    void FillBd(QWidget *parent, QString Name, QString Value);

signals:
    //    void SendCom45(quint32);
    //    void SendCom50(quint32 adr, float data);
    //    void RS485WriteCorBd(ModBus::Information, float *);
    //    void RS485ReadCorBd(ModBus::Information);
    //    void CorReadRequest();

public slots:
    //    void GetCorBdButton() override;
    void WriteCorBd() override;
    void WriteCor() override;
    void ResetCor() override;
    void SaveToFile() override;
    void ReadFromFile() override;
};

#endif // CORDIALOGKDV_H
