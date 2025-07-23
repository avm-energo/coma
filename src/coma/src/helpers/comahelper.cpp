#include <gen/logger.h>
#include <helpers/comahelper.h>
#include <helpers/constants.h>
#include <oscillograms/oscmanager.h>
#include <oscillograms/trendview/trendviewdialog.h>
#include <widgets/filefunc.h>
#include <widgets/graphfunc.h>

#include <QFile>
#include <QFont>
#include <QPainter>
#include <iostream>

ComaHelper::ComaHelper(QWidget *parent) : QObject(), m_parent(parent) { }

void ComaHelper::initAppSettings(const QString &appName, const QString &orgName, const QString &version)
{
    QCoreApplication::setApplicationName(appName);
    QCoreApplication::setOrganizationName(orgName);
    QCoreApplication::setApplicationVersion(version);
    Logger::writeStart("coma.log");
    qInstallMessageHandler(Logger::messageHandlerWithErrorQueue);
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
            loadOsc(filepath);
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
    painter.drawPixmap(QRect(20, 0, 20, 20), GraphFunc::NewCircle(Qt::red, 20));
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

void ComaHelper::loadOsc()
{
    auto filepath = FileFunc::ChooseFileForOpen(m_parent, "Oscillogram files (*.osc)");
    if (filepath.isEmpty())
        return;
    loadOsc(filepath);
}

void ComaHelper::loadSwj() { }

void ComaHelper::loadJournal() { }

void ComaHelper::loadOsc(const QString &filename)
{
    OscManager manager;
    OscManager::OscReturnType oscRet;
    if (!manager.loadOsc(filename, oscRet))
        return;

    TrendViewDialog *trendDialog = new TrendViewDialog(m_parent);
    trendDialog->setModel(oscRet.osc);
    trendDialog->setRange(oscRet.xmin, oscRet.xmax, oscRet.ymin, oscRet.ymax);
    trendDialog->setupPlots();
    trendDialog->setupUI();
    trendDialog->showPlot();
    trendDialog->show();
}

void ComaHelper::loadOsc(TrendViewModel *model)
{
    OscManager manager;
    OscManager::OscReturnType oscRet;
    if (!manager.loadOsc(model, oscRet))
        return;
}

void ComaHelper::loadSwj(const QString &filename) { }

void ComaHelper::loadJournal(const QString &filename) { }
