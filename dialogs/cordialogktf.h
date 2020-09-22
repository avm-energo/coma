#ifndef CORDIALOGKTF_H
#define CORDIALOGKTF_H

#include "../dialogs/abstractcordialog.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"
#include "../models/etablemodel.h"

#include <QByteArray>
#include <QDialog>

class CorDialogKTF : public AbstractCorDialog
{
    Q_OBJECT

public:
    explicit CorDialogKTF(QWidget *parent = nullptr);
    ~CorDialogKTF();

    //    int corDIndex;
    //    bool ok;
    //    int first;
    //    QTimer *MessageTimer;

    // int WriteCheckPassword();
    void GetCorBd(int index);

private:
    struct WBd7
    {
        float InitAge;
    };

    struct Bd9
    {
        float Vst;
        float Age;
        float Resurs;
        float reserv;
    };

    Bd9 *Bd9Block;
    WBd7 *WBd7Block;

    void FillCor();
    void FillBackCor();
    float ToFloat(QString text);
    void SetupUI();
    void FillBd(QWidget *parent, QString Name, QString Value);

signals:

public slots:
    void GetCorBdButton();
    void WriteCorBd();
    void WriteCor();
    void ResetCor();
    void SaveToFile();
    void ReadFromFile();

private slots:
};

#endif // CORDIALOGKTF_H
