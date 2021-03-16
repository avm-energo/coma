#include "servicemodule.h"

#include "../check/checkkdvdialog.h"
#include "../check/checkkdvharmonicdialog.h"
#include "../check/checkkdvvibrdialog.h"
#include "../check/checkkivdialog.h"
#include "../check/checkktfdialog.h"
#include "../check/checkktfharmonicdialog.h"
#include "../config/configkiv.h"
#include "../config/confkdvdialog.h"
#include "../config/confkivdialog.h"
#include "../config/confktfdialog.h"
#include "../dialogs/journalsdialog.h"
#include "../gen/board.h"
#include "../module/journkdv.h"
#include "../module/journkiv.h"
#include "../module/journktf.h"
#include "../module/modules.h"
#include "../startup/startupkdvdialog.h"
#include "../startup/startupkivdialog.h"
#include "../startup/startupktfdialog.h"

ServiceModule::ServiceModule(QObject *parent) : Module(parent)
{
}

void ServiceModule::create(QTimer *updateTimer)
{
    using namespace Modules;
    const auto &board = Board::GetInstance();
    quint16 typeb = board.typeB();
    if (BaseBoards.contains(typeb)) // there must be two-part module
    {
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

void ServiceModule::createModule(Modules::Model model)
{
    using namespace Modules;
    const auto &board = Board::GetInstance();
    switch (model)
    {
    case Model::KIV:
    {
        auto JOUR = UniquePointer<Journals>(new JournKIV(this));
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            ConfigKIV *CKIV = new ConfigKIV;
            addDialogToList(new ConfKIVDialog(CKIV), "Конфигурирование", "conf1");
        }
        CheckKIVDialog *cdkiv = new CheckKIVDialog;
        addDialogToList(cdkiv, "Проверка");

        addDialogToList(new StartupKIVDialog, "Начальные\nзначения");
        Module::create(std::move(JOUR));
        break;
    }
    case Model::KTF:
    {
        auto JOUR = UniquePointer<Journals>(new JournKTF(this));
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            ConfigKTF *CKTF = new ConfigKTF;
            addDialogToList(new ConfKTFDialog(CKTF), "Конфигурирование", "conf1");
        }
        CheckKTFDialog *cdktf = new CheckKTFDialog;
        addDialogToList(cdktf, "Проверка");

        addDialogToList(new StartupKTFDialog, "Старение\nизоляции");

        addDialogToList(new CheckKTFHarmonicDialog, "Гармоники");
        Module::create(std::move(JOUR));
        break;
    }
    case Model::KDV:
    {
        auto JOUR = UniquePointer<Journals>(new JournKDV(this));
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            ConfigKDV *CKDV = new ConfigKDV;
            addDialogToList(new ConfKDVDialog(CKDV), "Конфигурирование", "conf1");
        }
        CheckKDVDialog *cdkdv = new CheckKDVDialog;
        addDialogToList(cdkdv);

        addDialogToList(new StartupKDVDialog, "Старение\nизоляции");
        addDialogToList(new CheckKDVHarmonicDialog, "Гармоники");
        addDialogToList(new CheckKDVVibrDialog, "Вибрации");
        Module::create(std::move(JOUR));
        break;
    }
    default:
        assert(false);
    }
}

void ServiceModule::create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
{
    Q_UNUSED(typeB);
    Q_UNUSED(typeM);
    qCritical() << tr("Неизвестный модуль");
}
