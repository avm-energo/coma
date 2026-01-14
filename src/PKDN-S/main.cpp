#include "../gen/colors.h"
#include "../gen/stdfunc.h"
#include "config.h"
#include "pkdn_s.h"
#include <gen/settings.h>

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QString Parameter;
    QApplication a(argc, argv);
    StdFunc::Init();
    Settings::initialize(PROGNAME, SOFTDEVELOPER, COMAVERSION);
    a.setStyleSheet("QWidget { background: " + QString(MAINWINCLRA1) + "; color: " + QString(BLKCOLOR)
        + "; margin: 2px; }"); // Все виджеты унаследуют
    pkdn_s w;
    //    w.setAttribute(Qt::WA_AlwaysShowToolTips);
    w.SetMode(MainWindow::COMA_GENERALMODE);

#if PROGSIZE == PROGSIZE_EMUL
    w.SetMode(Coma::COMA_AUTON_MODE);
#endif
    w.Go();

    return a.exec();
}
