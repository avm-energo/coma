#ifndef CONFDIALOG_H
#define CONFDIALOG_H

#include "abstractconfdialog.h"

class ConfDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    ConfDialog(S2ConfigType *S2Config, quint32 MTypeB, quint32 MTypeM, QWidget *parent = nullptr);

    void SetDefConf() override;
    void Fill() override;
    void FillBack() override;
    void CheckConf() override;
    QWidget *SetupMainBlk(QObject *parent);
    QWidget *SetupTime(QObject *parent);

protected:
    void SetupUI() override;

private:
    Config *ConfigMain;
    QString ValuesFormat, WidgetFormat;
    QList<QWidget *> WidgetList;
    QObject *ParentMainbl, *ParentCtype;

    // AbstractConfDialog interface
};

#endif // CONFDIALOG_H
