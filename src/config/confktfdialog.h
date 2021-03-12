#ifndef CONFDIALOGKTF_H
#define CONFDIALOGKTF_H

#include "../config/configktf.h"
#include "abstractconfdialog.h"

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

class ConfKTFDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfKTFDialog(ConfigKTF *cktf, QWidget *parent = nullptr);
    ~ConfKTFDialog();

    //    int getRCount();

private:
    const QStringList Rates { "256", "128", "64", "32", "16" };

    bool DDosc;
    bool Mb;
    bool IsNtp;

    void Fill() override;
    void FillBack() override;
    void SetupUI() override;
    void CheckConf() override;

protected:
    QWidget *analogWidget();
    QWidget *transformerWidget();
    QWidget *alarmWidget();
    QWidget *ocsillogramWidget();
    virtual QWidget *connectionWidget();
    QWidget *otherWidget();

    void FillKtf();
    void FillBackKtf();

    ConfigKTF *ConfKTF;

private slots:
    void SetDefConf() override;
};

#endif // CONFDIALOGKTF_H
