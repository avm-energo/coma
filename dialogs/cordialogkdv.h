#ifndef CORDIALOGKDV_H
#define CORDIALOGKDV_H

#include "../dialogs/abstractcordialog.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"
#include "../models/etablemodel.h"

#include <QByteArray>
#include <QDialog>
class CorDialogKDV : public AbstractCorDialog
{
    Q_OBJECT
public:
    CorDialogKDV(QWidget *parent = nullptr);
    ~CorDialogKDV();

    // int corDIndex;
    // int first;

    void GetCorBd(int index);

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

    void FillBackWBd7();
    void FillBd9();
    void FillBackWBd8();

    float ToFloat(QString text);
    void SetupUI();
    void FillBd(QWidget *parent, QString Name, QString Value);

signals:
    void SendCom45(quint32);
    void SendCom50(quint32 adr, float data);
    void RS485WriteCorBd(ModBus::Information, float *);
    void RS485ReadCorBd(ModBus::Information);
    void CorReadRequest();

public slots:
    void GetCorBdButton();
    void WriteCorBd();
    void WriteCor();
    void ResetCor();
    void SaveToFile();
    void ReadFromFile();
};

#endif // CORDIALOGKDV_H
