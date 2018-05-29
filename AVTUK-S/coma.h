#ifndef COMA_H
#define COMA_H

#include <QMainWindow>
#include <QResizeEvent>
#include "../config/confdialog.h"
#include "../gen/mainwindow.h"

class Coma : public MainWindow
{
    Q_OBJECT

public:
    Coma(QWidget *parent = 0);
    ~Coma();

/*    void SetMode(int mode);
    int GetMode();
    void Go(const QString &parameter="");

    enum Modes
    {
        COMA_GENERALMODE, // обычный режим
        COMA_AUTON_OSCMODE, // автономный режим с отображением сохранённой осциллограммы
        COMA_AUTON_PROTMODE, // автономный режим с отображением протокола из прибора
        COMA_AUTON_SWJMODE, // автономный режим с отображением сохранённого журнала
        COMA_AUTON_MODE // просто автономный режим
    }; */

signals:

public slots:

private slots:
    void Emul2x();
    void Emul8x();

private:
    const QVector<int> MTBs = {0x21, 0x22, 0x31, 0x35};
//    int Mode; // режим запуска программы

    void SetupUI();
    void AddEmulToToolbar(QToolBar *tb);
    void AddActionsToMenuBar(QMenuBar *menubar);
    void Stage3();
    void PrepareDialogs();
};

#endif // COMA_H
