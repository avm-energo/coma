#include <common/appconfig.h>
#include <common/comahelper.h>
#include <common/constants.h>
#include <libavm-gen/settings.h>
#include <libavm-widgets/filefunc.h>
#include <libavm-widgets/graphfunc.h>
#include <oscillograms/dialogs/trendviewdialog.h>

#include <QCoreApplication>
#include <QDir>
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
    deployPackagedSettings();
}

/// \brief Копирует XML-описания модулей и шаблоны отчётов из каталога пакета
/// (<prefix>/share/<app>/) в пользовательский Settings::configDir().
/// \details На Windows это делает установщик (iss), а Linux-пакет ставится в систему
/// и писать в $HOME не может. При первом запуске новой версии файлы перезаписываются
/// (как и при установке на Windows), в остальных случаях докладываются только недостающие.
/// Если каталога пакета нет (запуск из дерева сборки, Windows) - ничего не делает.
void ComaHelper::deployPackagedSettings()
{
    const QDir packageDir(QCoreApplication::applicationDirPath() + "/../share/" + QCoreApplication::applicationName());
    if (!packageDir.exists())
        return;

    constexpr auto deployedVersionKey = "deployedSettingsVersion";
    const QString deployedVersion = Settings::get(deployedVersionKey, "");
    const bool overwrite = (deployedVersion != QCoreApplication::applicationVersion());

    const QList<QPair<QString, QString>> dirs {
        { "settings", Settings::configDir() },            //
        { "reports", Settings::configDir() + "reports/" } //
    };
    bool ok = true;
    for (const auto &[from, to] : dirs)
    {
        const QDir srcDir(packageDir.filePath(from));
        if (!srcDir.exists())
            continue;
        QDir().mkpath(to);
        for (const QString &name : srcDir.entryList(QDir::Files))
        {
            const QString dst = to + name;
            if (QFile::exists(dst))
            {
                if (!overwrite)
                    continue;
                QFile::remove(dst);
            }
            if (!QFile::copy(srcDir.filePath(name), dst))
            {
                qWarning() << "Can't copy" << srcDir.filePath(name) << "to" << dst;
                ok = false;
                continue;
            }
            // Файлы пакета принадлежат root и доступны только на чтение - копия должна быть редактируемой
            QFile::setPermissions(dst, QFile::permissions(dst) | QFileDevice::WriteOwner);
        }
    }
    if (ok && overwrite)
        Settings::set(deployedVersionKey, QCoreApplication::applicationVersion());
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
