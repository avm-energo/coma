#ifndef CONFIGDIALOGKXX_H
#define CONFIGDIALOGKXX_H

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/ecombobox.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "configkxx.h"

#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

class ConfDialogKxx : public QWidget
{
    Q_OBJECT
public:
    ConfDialogKxx(QVector<S2::DataRec> *S2Config, QWidget *parent = nullptr);

    QWidget *SetupComParam(QObject *parent);
    QWidget *SetupModBus(QObject *parent);
    QWidget *SetupBl(QObject *parent);
    void SetDefConf();
    void Fill();
    void FillBack();

    QStringList Sbaud = { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };

public slots:
    void ChangeWindow(int);

private:
    QString ValuesFormat, WidgetFormat;
    QList<QWidget *> WidgetList;
    ConfigKxx *Kxx;
    QObject *ParentMB, *ParentSetup, *ParentSetupBl;
};

#endif // CONFIGKXX_H
