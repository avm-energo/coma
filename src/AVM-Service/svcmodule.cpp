#include "svcmodule.h"

#include "../check/checkkdvdialog.h"
#include "../check/checkkdvharmonicdialog.h"
#include "../check/checkkdvvibrdialog.h"
#include "../check/checkkivdialog.h"
#include "../check/checkktfdialog.h"
#include "../check/checkktfharmonicdialog.h"
#include "../config/configdialog.h"
#include "../dialogs/journalsdialog.h"
#include "../gen/board.h"
#include "../module/journkdv.h"
#include "../module/journkiv.h"
#include "../module/journktf.h"
#include "../module/modules.h"
#include "../startup/startupkdvdialog.h"
#include "../startup/startupkivdialog.h"
#include "../startup/startupktfdialog.h"

SvcModule::SvcModule(QObject *parent) : Module(parent)
{
}

void SvcModule::create(QTimer *updateTimer)
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
    QList<UDialog *> dlgs = dialogs();
    for (auto *d : dlgs)
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
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            //   ConfigKIV *CKIV = new ConfigKIV;
            addDialogToList(new ConfigDialog(settings()->configSettings), "Конфигурирование", "conf1");
        }
        CheckKIVDialog *cdkiv = new CheckKIVDialog;
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
            //  ConfigKTF *CKTF = new ConfigKTF;
            addDialogToList(new ConfigDialog(settings()->configSettings), "Конфигурирование", "conf1");
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
            //  ConfigKDV *CKDV = new ConfigKDV;
            addDialogToList(new ConfigDialog(settings()->configSettings), "Конфигурирование", "conf1");
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
        assert(false);
    }
}

void SvcModule::create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
{
    Q_UNUSED(typeB);
    Q_UNUSED(typeM);
    qCritical() << tr("Неизвестный модуль");
}
