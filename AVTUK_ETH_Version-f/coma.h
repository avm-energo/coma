#ifndef COMA_H
#define COMA_H

#include <QMainWindow>
#include <QResizeEvent>
#include "../config/confdialog.h"
#include "../gen/mainwindow.h"
#include "../check/checkdialog84.h"
#include "../check/eabstractcheckdialog.h"
#include "../dialogs/cordialog.h"
#include "../dialogs/infodialog.h"

class Coma : public MainWindow
{
    Q_OBJECT

public:
    Coma(QWidget *parent = nullptr);
    ~Coma();

signals:

public slots:


private slots:

private:
    const QVector<int> MTBs = {0x21, 0x22, 0x31, 0x35, 0x80, 0x81, 0x84};
    CorDialog *CorD;
    InfoDialog *idlg;

    void SetupUI();
    void Stage3();
    void PrepareDialogs();
};

#endif // COMA_H
