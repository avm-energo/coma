#ifndef S_TQSPINBOX_H
#define S_TQSPINBOX_H

#include <QDoubleSpinBox>

class s_tqspinbox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit s_tqspinbox(QWidget *parent = 0);
    QVariant getAData();
    void setAData(QVariant dat);

signals:
    void valueChanged(double, s_tqspinbox *);

public slots:

private slots:
    void changedValue(double);

private:
    QVariant adata;
};

#endif // S_TQSPINBOX_H
