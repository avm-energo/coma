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

    QStringList Sbaud = { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };

    void SetDefConf();
    void Fill();
    void FillBack();
    void CheckConf();
    QWidget *SetupMainBlk(QObject *parent);
    QWidget *SetupTime(QObject *parent);

public slots:

private:
    QString ValuesFormat, WidgetFormat;
    QList<QWidget *> WidgetList;
    QObject *ParentMainbl, *ParentCtype;

private slots:

signals:
};

#endif // CONFDIALOG_H
