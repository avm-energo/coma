#include "egroupbox.h"

#include "../gen/colors.h"

EGroupBox::EGroupBox(QWidget *parent) : QGroupBox(parent)
{
    setStyleSheet("QGroupBox { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 "
        + QString(DCONFYCLR) + ", stop: 1 " + QString(DCONFGCLR)
        + ");"
          "border: 2px solid gray; border-radius: 5px; margin-top: 10px; }"
          "QGroupBox::title { subcontrol-origin: margin; left: 15px; padding: 0px 5px 0px 5px;}");
}

EGroupBox::EGroupBox(const QString &caption, QWidget *parent) : QGroupBox(caption, parent)
{
    setStyleSheet("QGroupBox { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 "
        + QString(DCONFYCLR) + ", stop: 1 " + QString(DCONFGCLR)
        + ");"
          "border: 2px solid gray; border-radius: 5px; margin-top: 10px; }"
          "QGroupBox::title { subcontrol-origin: margin; left: 15px; padding: 0px 5px 0px 5px;}");
}
