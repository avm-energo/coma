#ifndef CONFDIALOGKTF_H
#define CONFDIALOGKTF_H

#include "../config/configktf.h"
#include "abstractconfdialog.h"

class ConfKTFDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfKTFDialog(ConfigKTF *cktf, QWidget *parent = nullptr);
    ~ConfKTFDialog();

private:
    const QStringList Rates { "256", "128", "64", "32", "16" };

    bool DDosc;
    bool Mb;
    bool IsNtp;

    void Fill() override;
    void FillBack() const override;
    void SetupUI() override;
    void CheckConf() const override;

protected:
    QWidget *analogWidget();
    QWidget *transformerWidget();
    QWidget *alarmWidget();
    QWidget *ocsillogramWidget();
    virtual QWidget *connectionWidget();
    QWidget *otherWidget();

    void FillKtf();
    void FillBackKtf() const;

    ConfigKTF *ConfKTF;

private slots:
    void SetDefConf() override;
};

#endif // CONFDIALOGKTF_H
