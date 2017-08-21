#ifndef HIDDENDIALOG_H
#define HIDDENDIALOG_H

#include <QDialog>
#include <QPaintEvent>
#include <QVBoxLayout>
#include "../config/config.h"

#define RSTTIMEOUT  5000 // таймаут на рестарт модуля после отправки ему блока Bhb

class HiddenDialog : public QDialog
{
    Q_OBJECT
public:
    enum BM
    {
        BYMY,
        BNMY,
        BYMN,
        BNMN,
        PKDN
    };

    Config *HidConf;

    explicit HiddenDialog(int type, QWidget *parent = 0);
    void Fill();

signals:
    void accepted();

private:
    QString BGImage;
    int Type;

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
