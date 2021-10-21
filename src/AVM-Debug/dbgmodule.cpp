#include "dbgmodule.h"

#include "../avtuk/oscdialog.h"
#include "../avtuk/relaydialog.h"
#include "../avtuk/switchjournaldialog.h"
#include "../check/check3533dialog.h"
#include "../check/checkkdvdialog.h"
#include "../check/checkkdvharmonicdialog.h"
#include "../check/checkkdvvibrdialog.h"
#include "../check/checkkivdialog.h"
#include "../check/checkktfdialog.h"
#include "../check/checkktfharmonicdialog.h"
#include "../config/configdialog.h"
#include "../dialogs/hiddendialog.h"
#include "../dialogs/journalsdialog.h"
#include "../dialogs/timedialog.h"
#include "../module/journkdv.h"
#include "../module/journkiv.h"
#include "../module/journktf.h"
#include "../startup/startupkdvdialog.h"
#include "../startup/startupkivdialog.h"
#include "../startup/startupktfdialog.h"
#include "../tune/84/tune84dialog.h"
#include "../tune/kiv/tunekivdialog.h"
DbgModule::DbgModule(QObject *parent) : Module(parent)
{
}

void DbgModule::createModule(Modules::Model model)
{
    using namespace Modules;
    const auto &board = Board::GetInstance();
    switch (model)
    {
    case Model::KIV:
    {
        auto jour = UniquePointer<Journals>(new JournKIV(settings()->journals));
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            addDialogToList(
                new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
            if (board.interfaceType() == Board::InterfaceType::USB)
            {

                addDialogToList(new TuneKIVDialog(&configV), "Регулировка");
            }
        }
        CheckKIVDialog *cdkiv = new CheckKIVDialog;
        cdkiv->setHighlights(AbstractCheckDialog::Warning, settings()->highlightWarn);
        cdkiv->setHighlights(AbstractCheckDialog::Critical, settings()->highlightCrit);
        addDialogToList(cdkiv, "Проверка");

        addDialogToList(new StartupKIVDialog, "Начальные\nзначения");
        Module::create(std::move(jour));

        break;
    }
    case Model::KTF:
    {
        auto jour = UniquePointer<Journals>(new JournKTF(settings()->journals, this));
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            addDialogToList(
                new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
        }
        CheckKTFDialog *cdktf = new CheckKTFDialog;
        addDialogToList(cdktf, "Проверка");
        addDialogToList(new CheckKTFHarmonicDialog, "Гармоники");
        addDialogToList(new StartupKTFDialog, "Старение\nизоляции");

        Module::create(std::move(jour));
        break;
    }
    case Model::KDV:
    {
        auto jour = UniquePointer<Journals>(new JournKDV(settings()->journals, this));
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            addDialogToList(
                new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
        }
        CheckKDVDialog *cdkdv = new CheckKDVDialog;
        addDialogToList(cdkdv, "Проверка");
        addDialogToList(new CheckKDVHarmonicDialog, "Гармоники");
        addDialogToList(new CheckKDVVibrDialog, "Вибрации");
        addDialogToList(new StartupKDVDialog, "Старение\nизоляции");
        Module::create(std::move(jour));
        break;
    }
    default:
        break;
        //  assert(false);
    }
    TimeDialog *tdlg = new TimeDialog;
    addDialogToList(tdlg, "Время", "time");
}

void DbgModule::create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
{
    using namespace Modules;
    const auto &board = Board::GetInstance();
    if ((typeB == BaseBoard::MTB_80) && (typeM == MezzanineBoard::MTM_84))
    {
        qDebug("Here is KIV");
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            addDialogToList(
                new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
            if (board.interfaceType() == Board::InterfaceType::USB)
            {
                addDialogToList(new Tune84Dialog(&configV), "Регулировка");
            }
        }
        CheckKIVDialog *cdkiv = new CheckKIVDialog;
        addDialogToList(cdkiv, "Проверка");

        addDialogToList(new StartupKIVDialog, "Начальные\nзначения");
    }
    if ((typeB == BaseBoard::MTB_86) && (typeM == MezzanineBoard::MTM_00))
    {
        qDebug("Here is AVTUK-8600");
        addDialogToList(new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
        addDialogToList(new CheckKDVVibrDialog, "Вибрации");
    }
    if ((typeB == BaseBoard::MTB_80) && (typeM == MezzanineBoard::MTM_82))
    {
        qDebug("Here is AVTUK-8082");
        addDialogToList(new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
        addDialogToList(new OscDialog, "Осциллограммы");
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
        addDialogToList(new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
        addDialogToList(new SwitchJournalDialog, "Журнал переключений");
        addDialogToList(new OscDialog, "Осциллограммы");
    }
    if (Board::isUSIO(typeB, typeM))
        createUSIO(typeB, typeM);
}

void DbgModule::createUSIO(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
{
    using namespace Modules;
    if ((typeB == BaseBoard::MTB_35) && (typeM == MezzanineBoard::MTM_33))
    {
        qDebug("Here is AVTUK-3533");
        addDialogToList(new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
        Check3533Dialog *check = new Check3533Dialog;
        addDialogToList(check, "Проверка");
        // addDialogToList(new OscDialog, "Осциллограммы");
    }
    if ((typeB == BaseBoard::MTB_35) && (typeM == MezzanineBoard::MTM_31))
    {
        qDebug("Here is AVTUK-3533");
        addDialogToList(new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
        //   Check3533Dialog *check = new Check3533Dialog;
        //   addDialogToList(check, "Проверка");
        // addDialogToList(new OscDialog, "Осциллограммы");
    }
    if (typeB == BaseBoard::MTB_35)
    {
        addDialogToList(new RelayDialog(4), "Реле", "relay1");
    }
}

void DbgModule::create(QTimer *updateTimer)
{
    using namespace Modules;
    auto &board = Board::GetInstance();
    quint16 typeb = board.typeB();
    if (BaseBoards.contains(typeb)) // there must be two-part module
    {
        board.setDeviceType(Board::Controller);
        quint16 typem = board.typeM();
        Q_UNUSED(typem)
        switch (typeb)
        {
        case BaseBoard::MTB_00:
            /*
                str = (checkMDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nБазовая";
                if (checkBDialog != nullptr)
                {
                    checkBDialog->setMinimumHeight(500);
                    MainTW->addTab(checkBDialog, str);
                    CheckIndex = MainTW->indexOf(checkBDialog);
                }
                str = (checkBDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nМезонин";
                if (checkMDialog != nullptr)
                    MainTW->addTab(checkMDialog, str);
            */
        default:
            create(static_cast<BaseBoard>(typeb), static_cast<MezzanineBoard>(typem));
            break;
        }
    }
    else
    {
        board.setDeviceType(Board::Module);
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
