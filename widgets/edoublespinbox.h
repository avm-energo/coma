#ifndef EDOUBLESPINBOX_H
#define EDOUBLESPINBOX_H

#include <QSpinBox>

class EDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    EDoubleSpinBox(QWidget *parent = 0);

protected:
    virtual void wheelEvent(QWheelEvent *event);
};

#endif // EDOUBLESPINBOX_H
