#include "dbgmodule.h"

#include "../dialogs/checkdialog.h"
#include "../dialogs/configdialog.h"
#include "../dialogs/hiddendialog.h"
#include "../dialogs/journalsdialog.h"
#include "../dialogs/plotdialog.h"
#include "../dialogs/relaydialog.h"
#include "../dialogs/switchjournaldialog.h"
#include "../dialogs/timedialog.h"
#include "../module/journkdv.h"
#include "../module/journkiv.h"
#include "../module/journktf.h"
#include "../oscillograms/oscdialog.h"
#include "../startup/startupkdvdialog.h"
#include "../startup/startupkivdialog.h"
#include "../startup/startupktfdialog.h"
#include "../tune/82/tune82dialog.h"
#include "../tune/84/tune84dialog.h"
#include "../tune/kiv/tunekivdialog.h"

DbgModule::DbgModule(QObject *parent) : QObject(parent)
{
}

void DbgModule::createModule(Modules::Model model)
{
    using namespace Modules;
    switch (model)
    {
    case Model::KIV:
    {
        auto jour = UniquePointer<Journals>(new JournKIV(settings()->journals));

        if (settings())
        {
            if (!settings()->configSettings.general.isEmpty())
            {
                addDialogToList(
                    new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
            }
            if (settings()->ifaceSettings.settings.isValid())
            {
                assert(m_gsettings.check.items.size() == 1);
                auto &&item = m_gsettings.check.items.front();
                addDialogToList(new TuneKIVDialog(&configV), "Регулировка");
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
            if (!settings()->configSettings.general.isEmpty())
            {
                addDialogToList(
                    new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
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
            if (!settings()->configSettings.general.isEmpty())
            {
                addDialogToList(
                    new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
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
        break;
    }
    TimeDialog *tdlg = new TimeDialog;
    addDialogToList(tdlg, "Время", "time");
}

void DbgModule::create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
{
    using namespace Modules;
    if (Board::isUSIO(typeB, typeM))
        return createUSIO(typeB, typeM);

    if (settings())
    {
        if (!settings()->configSettings.general.isEmpty())
        {
            addDialogToList(
                new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
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

    if ((typeB == BaseBoard::MTB_80) && (typeM == MezzanineBoard::MTM_84))
    {
        qDebug("Here is KIV");

        addDialogToList(new Tune84Dialog(&configV), "Регулировка");

        assert(m_gsettings.check.items.size() == 1);
        auto &&item = m_gsettings.check.items.front();
        auto check = new CheckDialog(item, m_gsettings.check.categories);
        check->setHighlights(CheckDialog::Warning, settings()->highlightWarn);
        check->setHighlights(CheckDialog::Critical, settings()->highlightCrit);
        addDialogToList(check, item.header, "check:" + item.header);

        addDialogToList(new StartupKIVDialog, "Начальные\nзначения");
    }
    if ((typeB == BaseBoard::MTB_86) && (typeM == MezzanineBoard::MTM_00))
    {
        qDebug("Here is AVTUK-8600");
    }
    if (typeB == BaseBoard::MTB_80)
    {
        if ((typeM == MezzanineBoard::MTM_81) || (typeM == MezzanineBoard::MTM_82) || (typeM == MezzanineBoard::MTM_83))
        {
            addDialogToList(new OscDialog, "Осциллограммы");
            addDialogToList(new Tune82Dialog(&configV, typeM), "Регулировка");
        }
    }
    if ((typeB == BaseBoard::MTB_80) && (typeM == MezzanineBoard::MTM_85))
    {
        qDebug("Here is AVTUK-8085");
        addDialogToList(new SwitchJournalDialog, "Журнал переключений");
        addDialogToList(new OscDialog, "Осциллограммы");
    }
    if ((typeB == BaseBoard::MTB_85) && (typeM == MezzanineBoard::MTM_85))
    {
        qDebug("Here is AVTUK-8585");
        addDialogToList(new SwitchJournalDialog, "Журнал переключений");
        addDialogToList(new OscDialog, "Осциллограммы");
    }
    if (!settings()->ifaceSettings.settings.isNull())
    {
        TimeDialog *tdlg = new TimeDialog;
        addDialogToList(tdlg, "Время", "time");
    }
}

void DbgModule::createUSIO(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
{
    using namespace Modules;
    QString message("Here is AVTUK-%1%2");
    qDebug() << message.arg(typeB, 0, 16).arg(typeM, 0, 16);
    if (!settings()->configSettings.general.empty())
    {
        addDialogToList(new ConfigDialog(&configV, settings()->configSettings.general),
            "Конфигурирование"
            "\nобщая",
            "confGeneral");
    }
    if (!settings()->configSettings.base.empty())
    {
        addDialogToList(new ConfigDialog(&configV, settings()->configSettings.base, false),
            "Конфигурирование"
            "\nбазовая",
            "confBase");
    }
    if (!settings()->configSettings.mezz.empty())
    {
        addDialogToList(new ConfigDialog(&configV, settings()->configSettings.mezz, false),
            "Конфигурирование"
            "\nмезонинная",
            "confMezz");
    }

    if ((!settings()->ifaceSettings.settings.isValid())
        && (Board::GetInstance().interfaceType() != Board::InterfaceType::Emulator))
        return;

    const auto &item = m_gsettings.check.items.at(0);
    if (typeB == BaseBoard::MTB_35)
    {
        addDialogToList(new RelayDialog(4), "Реле", "relay1");
    }

    addDialogToList(new CheckDialog(item, m_gsettings.check.categories), item.header, "check:" + item.header);
    if (m_gsettings.check.items.size() == 2)
    {
        const auto &itemIn = m_gsettings.check.items.at(1);
        addDialogToList(new CheckDialog(itemIn, m_gsettings.check.categories), itemIn.header, "check:" + itemIn.header);
    }
}

void DbgModule::create(QTimer *updateTimer)
{
    using namespace Modules;
    BaseInterface::iface()->setSettings(settings()->ifaceSettings);
    auto &board = Board::GetInstance();

    quint16 typeb = board.typeB();
    if (BaseBoards.contains(typeb)) // there must be two-part module
    {
        quint16 typem = board.typeM();
        create(static_cast<BaseBoard>(typeb), static_cast<MezzanineBoard>(typem));
    }
    else
    {
        quint16 mtype = board.type();
        createModule(Modules::Model(mtype));
    }
    createCommon();
    HiddenDialog *hiddenDialog = new HiddenDialog;
    hiddenDialog->fill();
    addDialogToList(hiddenDialog, "Секретные операции", "hidden");
    QList<UDialog *> dlgs = dialogs();
    for (auto *d : dlgs)
    {
        connect(updateTimer, &QTimer::timeout, d, &UDialog::reqUpdate);
        d->uponInterfaceSetting();
    }
}
