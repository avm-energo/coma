#ifndef CONFDIALOG_H
#define CONFDIALOG_H

#include "confdialogkxx.h"
#include "config.h"
#include "configkxx.h"

#include <QDialog>
#include <QWidget>

class ConfDialog : public QWidget
{
    Q_OBJECT
public:
    ConfDialog(QVector<S2::DataRec> *S2Config, quint32 MTypeB, quint32 MTypeM, QWidget *parent = nullptr);

    Config *ConfigMain;
    ConfDialogKxx *ConfKxx;
    ConfigKxx *Kxx;

    QStringList Sbaud = { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };

    void SetDefConf();
    void Fill();
    void FillMainBlk();
    void FillBack();
    void CheckConf();
    QWidget *SetupMainBlk(QWidget *parent);
    //    QWidget *SetupComParam();
    //   QWidget *SetupBl(QWidget *parent);

public slots:
    //  void ChangeWindow(int);

private:
    QString ValuesFormat, WidgetFormat;
    QList<QWidget *> WidgetList;
private slots:
    //   void Set104(double);
    // void SetCType(int);

signals:
};

#endif // CONFDIALOG_H
