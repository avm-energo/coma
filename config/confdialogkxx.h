#ifndef CONFIGDIALOGKXX_H
#define CONFIGDIALOGKXX_H

#include "abstractconfdialog.h"
#include "configkxx.h"

class ConfDialogKxx : public AbstractConfDialog
{
    Q_OBJECT
public:
    ConfDialogKxx(S2ConfigType *S2Config, QWidget *parent = nullptr);

    QWidget *SetupComParam(QObject *parent);
    QWidget *SetupModBus(QObject *parent);
    QWidget *SetupBl(QObject *parent);
    void SetDefConf() override;
    void Fill() override;
    void FillBack() override;
    void CheckConf() override;

public slots:
    void ChangeWindow(int);

private:
    QString ValuesFormat, WidgetFormat;
    QList<QWidget *> WidgetList;
    ConfigKxx *Kxx;
    QObject *ParentMB, *ParentSetup, *ParentSetupBl;
    void SetupUI() override;

    // AbstractConfDialog interface
};

#endif // CONFIGKXX_H
