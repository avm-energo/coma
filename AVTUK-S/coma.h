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
    const QVector<int> MTBs = {0x21, 0x22, 0x31, 0x35};

    void SetupUI();
    void AddActionsToMenuBar(QMenuBar *menubar);
    void Stage3();
    void PrepareDialogs();
};

#endif // COMA_H
