#include "s_tqcheckbox.h"

s_tqCheckBox::s_tqCheckBox(QWidget *parent) :
    QCheckBox(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
}
