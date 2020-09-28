#include "../gen/logger.h"
#include "coma.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
#ifdef QT_DEBUG
    qSetMessagePattern("%{file}(%{line}): %{message}");
#endif
    QString Parameter;
    QApplication a(argc, argv);
    // Следует поменять на ifndef
    //#ifdef QT_DEBUG
    //    QDirIterator it(QDir::currentPath(), { "*.txt" });
    //    while (it.hasNext())
    //        QFile(it.next()).remove();
    //    qInstallMessageHandler(Logging::messageHandler);
    //#endif
    a.setStyle("cleanlooks");
    Coma w;
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
