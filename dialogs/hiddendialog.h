#ifndef HIDDENDIALOG_H
#define HIDDENDIALOG_H

#include <QDialog>
#include <QPaintEvent>
#include <QVBoxLayout>
#include "../config/config.h"

#define RSTTIMEOUT  15000 // таймаут на рестарт модуля после отправки ему блока Bhb

#define BNMN    0x00
#define BNMY    0x01
#define BYMN    0x02
#define BYMY    0x03

class HiddenDialog : public QDialog
{
    Q_OBJECT
public:
    Config *HidConf;

    explicit HiddenDialog(QWidget *parent = 0);
    void Fill();

signals:
    void accepted();

private:
    struct Bhb_Main
    {
        quint32 MType;          // тип платы
        quint32 SerialNum;      // серийный номер платы
        quint32 HWVer;          // версия аппаратного обеспечения модуля (платы)
        quint32 ModSerialNum;   // серийный номер модуля целиком
    };
    QString BGImage;
    int Type;
    struct Bhb_Overall
    {
        Bhb_Main BoardBBhb;
        Bhb_Main BoardMBhb;
    };
    Bhb_Overall Bhb;

    void SetupUI();
    void SetVersion(quint32 number, QString lename);
    void GetVersion(quint32 &number, QString lename);
    void SendBhb();

private slots:
    void AcceptChanges();

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // HIDDENDIALOG_H
