#include "../gen/logger.h"
#include "config.h"
#include "servicecoma.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QString Parameter;
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName(PROGNAME);
    QCoreApplication::setOrganizationName(SOFTDEVELOPER);
    QCoreApplication::setApplicationVersion(COMAVERSION);
    Q_INIT_RESOURCE(style);
    Q_INIT_RESOURCE(styles);
    Q_INIT_RESOURCE(vectorIcons);
    Q_INIT_RESOURCE(settings);
    Q_INIT_RESOURCE(helper);
    Logging::writeStart();
    qInstallMessageHandler(Logging::messageHandler);
    ServiceComa w;
    w.setMode(Coma::COMA_GENERALMODE);

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
                w.setMode(Coma::COMA_AUTON_OSCMODE);
            else if (filestail == "swj")
                w.setMode(Coma::COMA_AUTON_SWJMODE);
            else if (filestail == "vrf")
                w.setMode(Coma::COMA_AUTON_PROTMODE);
            else
            {
                w.setMode(Coma::COMA_AUTON_MODE);
                Parameter.clear();
            }
        }
    }

    w.go(Parameter);

    return a.exec();
}
