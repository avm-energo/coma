#include <QApplication>
#include <QCommandLineParser>
#include "pkdn_s.h"

int main(int argc, char *argv[])
{
    QString Parameter;
    QApplication a(argc, argv);
    pkdn_s w;
//    w.setAttribute(Qt::WA_AlwaysShowToolTips);
    w.SetMode(MainWindow::COMA_GENERALMODE);
    QCoreApplication::setApplicationName("PKDN-S");
    QCoreApplication::setApplicationVersion(COMAVERSION);
    if (argc > 1) // есть аргументы запуска
    {
        QCommandLineParser parser;
/*        parser.setApplicationDescription("COMA");
        parser.addHelpOption();
        parser.addVersionOption(); */
        parser.addPositionalArgument("file", "file with oscillogramm (*.osc), with switch journal (*.swj)\n" \
                                             "or with pkdn file (*.vrf)");
        parser.process(QCoreApplication::arguments());
        const QStringList files = parser.positionalArguments();
        // file is files.at(0)
        if (!files.isEmpty())
        {
            Parameter = files.at(0);
            QString filestail = Parameter.right(3);
            if (filestail == "osc")
                w.SetMode(MainWindow::COMA_AUTON_OSCMODE);
            else if (filestail == "swj")
                w.SetMode(MainWindow::COMA_AUTON_SWJMODE);
            else if (filestail == "vrf")
                w.SetMode(MainWindow::COMA_AUTON_PROTMODE);
            else
            {
                w.SetMode(MainWindow::COMA_AUTON_MODE);
                Parameter.clear();
            }
        }
    }
#if PROGSIZE == PROGSIZE_EMUL
    w.SetMode(Coma::COMA_AUTON_MODE);
#endif
    w.Go(Parameter);

    return a.exec();
}
