#ifndef UDIALOG_H
#define UDIALOG_H

// simple dialog class to populate from

#include "uwidget.h"

class UDialog : public UWidget
{
    Q_OBJECT
public:
    UDialog(QWidget *parent = nullptr);
};

#endif // UDIALOG_H
