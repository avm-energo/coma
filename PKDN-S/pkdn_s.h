#ifndef PKDN_S_H
#define PKDN_S_H

#include <QResizeEvent>
#include "../mainwindow.h"

class pkdn_s : public MainWindow
{
    Q_OBJECT

public:
    pkdn_s(QWidget *parent = 0);
    void SetupUI();
    void Stage3();

signals:

private:
};

#endif // PKDN_S_H
