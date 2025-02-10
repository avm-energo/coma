#pragma once

#include <QSpinBox>
#include <QWheelEvent>

class ESpinBox : public QSpinBox
{
    Q_OBJECT
public:
    ESpinBox(QWidget *parent = nullptr);

protected:
    virtual void wheelEvent(QWheelEvent *event);
};
