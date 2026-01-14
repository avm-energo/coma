#include "epushbutton.h"

EPushButton::EPushButton(QWidget *parent) : QPushButton(parent)
{
    setStyleSheet("QPushButton {background-color: rgba(0,0,0,0); border: 1px solid gray; border-radius: 5px;}");
}

EPushButton::EPushButton(const QString &caption, QWidget *parent) : QPushButton(caption, parent)
{
    setStyleSheet("QPushButton {background-color: rgba(0,0,0,0); border: 1px solid gray; border-radius: 5px;}");
}
