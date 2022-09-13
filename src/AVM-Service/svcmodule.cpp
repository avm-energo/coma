#include "svcmodule.h"

#include "../dialogs/checkdialog.h"
#include "../dialogs/configdialog.h"
#include "../dialogs/journalsdialog.h"
#include "../dialogs/plotdialog.h"
#include "../dialogs/timedialog.h"
#include "../module/board.h"
#include "../module/journkdv.h"
#include "../module/journkiv.h"
#include "../module/journktf.h"
#include "../module/modules.h"
#include "../startup/startupkdvdialog.h"
#include "../startup/startupkivdialog.h"
#include "../startup/startupktfdialog.h"

#include <QMainWindow>
SvcModule::SvcModule(QObject *parent) : QObject(parent)
{
}

void SvcModule::create(QTimer *updateTimer)
{
    using namespace Modules;
    BaseInterface::iface()->setSettings(settings()->ifaceSettings);
    auto &board = Board::GetInstance();
    quint16 typeb = board.typeB();
    if (!BaseBoards.contains(typeb)) // there must be two-part module
    {
        board.setDeviceType(Board::Module);
        quint16 mtype = board.type();
        createModule(Modules::Model(mtype));
    }
    createCommon();
    QList<UDialog *> dlgs = dialogs();
    for (auto d : dlgs)
    {
        connect(updateTimer, &QTimer::timeout, d, &UDialog::reqUpdate);
        d->uponInterfaceSetting();
    }
}

void SvcModule::createModule(Modules::Model model)
{
    using namespace Modules;
    const auto &board = Board::GetInstance();
    switch (model)
    {
    case Model::KIV:
    {
        auto jour = UniquePointer<Journals>(new JournKIV(settings()->journals, this));

        if (settings())
        {
            if (!settings()->configSettings.general.isEmpty())
            {
                if (board.interfaceType() != Board::InterfaceType::RS485)
                {
                    addDialogToList(
                        new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
                }
            }
            if (settings()->ifaceSettings.settings.isValid())
            {
                assert(m_gsettings.check.items.size() == 1);
                auto &&item = m_gsettings.check.items.front();
                auto check = new CheckDialog(item, m_gsettings.check.categories);
                check->setHighlights(CheckDialog::Warning, settings()->highlightWarn);
                check->setHighlights(CheckDialog::Critical, settings()->highlightCrit);
                addDialogToList(check, item.header, "check:" + item.header);
            }
        }
        addDialogToList(new PlotDialog, "Диаграммы");
        addDialogToList(new StartupKIVDialog, "Начальные\nзначения");
        Module::create(std::move(jour));
        break;
    }
    case Model::KTF:
    {
        auto jour = UniquePointer<Journals>(new JournKTF(settings()->journals, this));

        if (settings())
        {
            if (board.interfaceType() != Board::InterfaceType::RS485)
            {
                if (!settings()->configSettings.general.isEmpty())
                {
                    addDialogToList(
                        new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
                }
            }
            if (settings()->ifaceSettings.settings.isValid())
            {
                for (auto &&item : m_gsettings.check.items)
                {
                    addDialogToList(
                        new CheckDialog(item, m_gsettings.check.categories), item.header, "check:" + item.header);
                }
            }
        }
        Module::create(std::move(jour));
        break;
    }
    case Model::KDV:
    {
        auto jour = UniquePointer<Journals>(new JournKDV(settings()->journals, this));

        if (settings())
        {
            if (board.interfaceType() != Board::InterfaceType::RS485)
            {
                if (!settings()->configSettings.general.isEmpty())
                {
                    addDialogToList(
                        new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
                }
            }
            if (settings()->ifaceSettings.settings.isValid())
            {
                for (auto &&item : m_gsettings.check.items)
                {
                    addDialogToList(
                        new CheckDialog(item, m_gsettings.check.categories), item.header, "check:" + item.header);
                }
            }
        }
        Module::create(std::move(jour));
        break;
    }
    default:
    {

        QString message = QString("Неизвестный модуль\n"
                                  "Прочитан некорретный BSI: typeB:%1, typeM:%2\n"
                                  "%3 не поддерживает такой модуль")
                              .arg(board.baseSerialInfo().MTypeB, 0, 16)
                              .arg(board.baseSerialInfo().MTypeM, 0, 16)
                              .arg(QCoreApplication::applicationName());
        QWidget *parent = qobject_cast<QWidget *>(WDFunc::getMainWindow());
        Q_CHECK_PTR(parent);
        if (parent)
            QMessageBox::warning(parent, "Некорретный BSI", message);
        return;
    }
    }
    addDialogToList(new TimeDialog, "Время", "time");
}

void SvcModule::create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
{
    Q_UNUSED(typeB);
    Q_UNUSED(typeM);
    qCritical() << tr("Неизвестный модуль");
}
