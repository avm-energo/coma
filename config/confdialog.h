#ifndef CONFDIALOG_H
#define CONFDIALOG_H

#include "config.h"

#include <QDialog>
#include <QWidget>

class ConfDialog : public QWidget
{
    Q_OBJECT
public:
    ConfDialog(QVector<S2::DataRec> *S2Config, quint32 MTypeB, quint32 MTypeM, QWidget *parent = nullptr);

    Config *ConfigMain;

    void SetDefConf();
    void Fill();
    void FillBack();
    void CheckConf();
    QWidget *SetupMainBlk(QWidget *parent);
    QWidget *SetupComParam(QWidget *parent);
    QWidget *SetupBl(QWidget *parent);

    QStringList Sbaud = QStringList() << "1200"
                                      << "2400"
                                      << "4800"
                                      << "9600"
                                      << "19200"
                                      << "38400"
                                      << "57600"
                                      << "115200";

    typedef struct
    {
        float RTerm;
        float W100;

    } StructDop;
    StructDop StrD;

    typedef struct
    {
        quint32 Trele_pred;
        quint32 Trele_alarm;

    } StructTrele;
    StructTrele StrTrele;

    typedef struct
    {
        quint8 MBMaster;
        quint8 MBMab1[4];
        quint8 MBMab2[4];
        quint8 MBMab3[4];
        quint8 MBMab4[4];

    } StructModBus;
    StructModBus StrModBus;

public slots:
    void ChangeWindow(int);

private:
    QString ValuesFormat, WidgetFormat;
    QList<QWidget *> WidgetList;
private slots:
    void Set104(double);
    void SetCType(int);

signals:
};

#endif // CONFDIALOG_H
