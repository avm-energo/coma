#ifndef CONFDIALOGKIV_H
#define CONFDIALOGKIV_H

#include "abstractconfdialog.h"

class ConfigKIV;
class ConfKIVDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfKIVDialog(ConfigKIV *ckiv, QWidget *parent = nullptr);
    ~ConfKIVDialog();

private:
    void Fill() override;
    void FillBack() const override;
    void SetupUI() override;
    void CheckConf() const override;

protected:
    QWidget *analogWidget();
    QWidget *thresholdsWidget();
    QWidget *remainsWidget();
    virtual QWidget *connectionWidget();

    void FillKiv();
    void FillBackKiv() const;

    ConfigKIV *CKIV;

private slots:
    void SetDefConf() override;
};

#endif // CONFDIALOG84_H
