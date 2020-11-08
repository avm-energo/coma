#ifndef UDIALOG_H
#define UDIALOG_H

// simple dialog class to populate from

#include "uwidget.h"

class UDialog : public UWidget
{
    Q_OBJECT
public:
    UDialog(QWidget *parent = nullptr);

    //    virtual void setConnections() = 0;

public slots:

private:
};

#endif // UDIALOG_H
