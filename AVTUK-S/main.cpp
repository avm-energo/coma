#include <QApplication>
#include <QCommandLineParser>
#include "coma.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Coma w;
    w.SetMode(Coma::COMA_GENERALMODE);
    QCoreApplication::setApplicationName("COMA");
    QCoreApplication::setApplicationVersion(COMAVERSION);
    if (argc > 1) // есть аргументы запуска
    {
        QCommandLineParser parser;
        parser.setApplicationDescription("COMA");
        parser.addHelpOption();
        parser.addVersionOption();
        parser.addPositionalArgument("file", "file with oscillogramm (*.osc) or with switch journal (*.swj)");
        const QStringList files = parser.positionalArguments();
        // file is files.at(0)
        if (!files.isEmpty())
        {
            QString file = files.at(0);
            QString filestail = file.right(3);
            if (filestail == "osc")
                w.SetMode(Coma::COMA_AUTON_OSCMODE);
            else if (filestail == "swj")
                w.SetMode(Coma::COMA_AUTON_SWJMODE);
            else if (filestail == "vrf")
                w.SetMode(Coma::COMA_AUTON_PROTMODE);
        }
    }
#if PROGSIZE == PROGSIZE_EMUL
    w.SetMode(Coma::COMA_AUTON_MODE);
#endif
    w.Go();

    return a.exec();
}

