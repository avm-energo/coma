#include <QApplication>
#include <QCommandLineParser>
#include "coma.h"

int main(int argc, char *argv[])
{
    QString Parameter;
    QApplication a(argc, argv);
    Coma w;
//    w.setAttribute(Qt::WA_AlwaysShowToolTips);
    w.SetMode(Coma::COMA_GENERALMODE);
    QCoreApplication::setApplicationName(PROGCAPTION);
    QCoreApplication::setApplicationVersion(COMAVERSION);
    if (argc > 1) // есть аргументы запуска
    {
        QCommandLineParser parser;
/*        parser.setApplicationDescription("COMA");
        parser.addHelpOption();
        parser.addVersionOption(); */
        parser.addPositionalArgument("file", "file with oscillogramm (*.osc) or with switch journal (*.swj)");
        parser.process(QCoreApplication::arguments());
        const QStringList files = parser.positionalArguments();
        // file is files.at(0)
        if (!files.isEmpty())
        {
            Parameter = files.at(0);
            QString filestail = Parameter.right(3);
            if (filestail == "osc")
                w.SetMode(Coma::COMA_AUTON_OSCMODE);
            else if (filestail == "swj")
                w.SetMode(Coma::COMA_AUTON_SWJMODE);
            else if (filestail == "vrf")
                w.SetMode(Coma::COMA_AUTON_PROTMODE);
            else
            {
                w.SetMode(Coma::COMA_AUTON_MODE);
                Parameter.clear();
            }
        }
    }
    w.Go(Parameter);

    return a.exec();
}
