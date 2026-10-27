#include "oscillograms/dialogs/switchjournaldialog.h"
#include <common/appconfig.h>
#include <common/comahelper.h>
#include <common/constants.h>
#include <libavm-widgets/filefunc.h>
#include <libavm-widgets/graphfunc.h>
#include <libavm-widgets/wdfunc.h>
#include <oscillograms/dialogs/trendviewdialog.h>
#include <oscillograms/swjmanager.h>

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

void ComaHelper::loadSwj(const QString &filename)
{
    SwjManager swjManager;
    OscManager oscManager;
    File::Vector fileVector = oscManager.loadFromFile(filename);
    auto oscVector = swjManager.loadFromFile(filename);
    std::move(oscVector.begin(), oscVector.end(), std::back_inserter(fileVector));
    SwjModel *swjModel = nullptr;
    TrendViewModel *oscModel = nullptr;
    for (auto &item : fileVector)
    {
        std::visit(
            overloaded {
                [&](S2::OscHeader &header) { oscManager.setHeader(header); }, //
                [&](SwjModel &model) { swjModel = &model; },                  //
                [&](TrendViewModel *model) { oscModel = model; }              //
            },
            item);
    }
    if (!swjModel || !oscModel)
        return;

    auto dialog = new SwitchJournalViewDialog(*swjModel, oscModel, oscManager);
    dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
    dialog->setMinimumHeight(WDFunc::getMainWindow()->height());
    dialog->setMinimumWidth(WDFunc::getMainWindow()->width());
    dialog->adjustSize();
}

void ComaHelper::loadJournal(const QString &filename) { }
