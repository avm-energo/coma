#ifndef COMA_H
#define COMA_H

#include <QMainWindow>
#include <QResizeEvent>
#include "../config/confdialog.h"
#include "../mainwindow.h"
#include "../publicclass.h"

class Coma : public MainWindow
{
    Q_OBJECT

public:
    Coma(QWidget *parent = 0);
    ~Coma();

signals:

public slots:

private slots:
    void Emul2x();
    void Emul8x();
    void EmulA1();
    void ProtocolFromFile();

private:
    void SetupUI();
    void AddActionsToMenuBar(QMenuBar *menubar);
    void Stage3();
    void PrepareDialogs();
};

#endif // COMA_H
