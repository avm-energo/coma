#ifndef EDOUBLESPINBOX_H
#define EDOUBLESPINBOX_H

#include <QDoubleSpinBox>

class EDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    EDoubleSpinBox(QWidget *parent = nullptr);

protected:
    virtual void wheelEvent(QWheelEvent *event);
};

#endif // EDOUBLESPINBOX_H
