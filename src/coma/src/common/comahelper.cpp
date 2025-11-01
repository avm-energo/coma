#include <common/appconfig.h>
#include <common/comahelper.h>
#include <common/constants.h>
#include <oscillograms/dialogs/trendviewdialog.h>
#include <avm-widgets/filefunc.h>
#include <avm-widgets/graphfunc.h>

#include <QFile>
#include <QFont>
#include <QPainter>
#include <iostream>

ComaHelper::ComaHelper() : QObject() { }

void ComaHelper::initAppSettings()
{
    MessageHandler::setMessageHandlerFilename("coma.log");
    MessageHandler::setQueueLevel(
        (AppConfiguration::app() == AppConfiguration::Service) ? Logger::LOGLEVEL_INFO : Logger::LOGLEVEL_DEBUG);
    qInstallMessageHandler(&MessageHandler::messageHandlerWithErrorQueue);
}

void ComaHelper::parseCommandLine()
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "file with oscillogramm (*.osc) or with switch journal (*.swj)");
    parser.process(QCoreApplication::arguments());
    const QStringList files = parser.positionalArguments();
    if (!files.isEmpty())
    {
        auto &filepath = files.at(0);
        QFileInfo fileInfo(filepath);
        auto ext = fileInfo.suffix();
        if (ext == "osc")
        {
            TrendViewDialog *dlg = new TrendViewDialog;
            dlg->loadOsc(filepath);
        }
        else if (ext == "swj")
            loadSwj(filepath);
        else if (ext.contains("jn"))
            loadJournal(filepath);
        // TODO
        // else if (filestail == "vrf")
        else
        {
            std::cout << "Wrong file" << std::endl;
            std::exit(0);
        }
    }
}

void ComaHelper::convertPixmap(size_t size, QAction *jourAct)
{
    if (!QFile::exists(Constants::Resources[Constants::FROSYA]))
        return;
    const QIcon jourIcon(Constants::Resources[Constants::FROSYA]);
    QPixmap pix = jourIcon.pixmap(QSize(40, 40), QIcon::Disabled);
    QPainter painter(&pix);
    painter.drawPixmap(QRect(20, 0, 20, 20), GraphFunc::newCircle(Qt::red, 20));
    QFont font(painter.font());
    font.setPixelSize(14);
    painter.setFont(font);
    painter.setPen(Qt::white);
    if (size > 99)
        painter.drawText(QRect(20, 0, 20, 20), Qt::AlignCenter, "...");
    else
        painter.drawText(QRect(20, 0, 20, 20), Qt::AlignCenter, QString::number(size));
    jourAct->setIcon(pix);
}

void ComaHelper::loadSwj() { }

void ComaHelper::loadJournal() { }

void ComaHelper::loadSwj(const QString &filename) { }

void ComaHelper::loadJournal(const QString &filename) { }
