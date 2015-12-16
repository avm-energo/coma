#ifndef s_tqSpinBox_H
#define s_tqSpinBox_H

#include <QDoubleSpinBox>

class s_tqSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit s_tqSpinBox(QWidget *parent = 0);
    QVariant getAData();
    void setAData(QVariant dat);

signals:
    void valueChanged(double, s_tqSpinBox *);

public slots:

private slots:
    void changedValue(double);

private:
    QVariant adata;
};

#endif // s_tqSpinBox_H
