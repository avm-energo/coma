#include "tunemodule.h"

#include "../check/checkkdvdialog.h"
#include "../check/checkkdvharmonicdialog.h"
#include "../check/checkkdvvibrdialog.h"
#include "../check/checkkivdialog.h"
#include "../check/checkktfdialog.h"
#include "../check/checkktfharmonicdialog.h"
#include "../config/confkdvdialog.h"
#include "../config/confkivdialog.h"
#include "../config/confktfdialog.h"
#include "../dialogs/journalsdialog.h"
#include "../module/journkdv.h"
#include "../module/journkiv.h"
#include "../module/journktf.h"
#include "../startup/startupkdvdialog.h"
#include "../startup/startupkivdialog.h"
#include "../startup/startupktfdialog.h"
#include "../tune/kiv/tunekivdialog.h"
TuneModule::TuneModule(QObject *parent) : Module(parent)
{
}

void TuneModule::create(Modules::Model model)
{
    using namespace Modules;
    UniquePointer<Journals> JOUR;
    const auto &board = Board::GetInstance();
    switch (model)
    {
    case Model::KIV:
    {
        JOUR = UniquePointer<Journals>(new JournKIV(this));
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            ConfigKIV *CKIV = new ConfigKIV;
            addDialogToList(new ConfKIVDialog(CKIV), "Конфигурирование", "conf1");
            if (board.interfaceType() == Board::InterfaceType::USB)
            {

                addDialogToList(new TuneKIVDialog(CKIV), "Регулировка");
            }
        }
        CheckKIVDialog *cdkiv = new CheckKIVDialog;
        addDialogToList(cdkiv, "Проверка");

        addDialogToList(new StartupKIVDialog, "Начальные\nзначения");

        break;
    }
    case Model::KTF:
    {
        JOUR = UniquePointer<Journals>(new JournKTF(this));
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            ConfigKTF *CKTF = new ConfigKTF;
            addDialogToList(new ConfKTFDialog(CKTF), "Конфигурирование", "conf1");
        }
        CheckKTFDialog *cdktf = new CheckKTFDialog;
        addDialogToList(cdktf);
        // TuneKTF *TKTF = new TuneKTF(0, s2Config);
        //                        m->addDialogToList(new TuneKTFDialog(CKTF, TKTF));

        addDialogToList(new StartupKTFDialog, "Старение\nизоляции");
        addDialogToList(new CheckKTFHarmonicDialog, "Гармоники");
        break;
    }
    case Model::KDV:
    {
        JOUR = UniquePointer<Journals>(new JournKDV(this));
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            ConfigKDV *CKDV = new ConfigKDV;
            addDialogToList(new ConfKDVDialog(CKDV), "Конфигурирование", "conf1");
        }
        CheckKDVDialog *cdkdv = new CheckKDVDialog;
        addDialogToList(cdkdv);
        //            TuneKDV *TKDV = new TuneKDV;
        //            m->addDialogToList(new TuneKDVDialog(CKDV, TKDV));

        addDialogToList(new StartupKDVDialog, "Старение\nизоляции");
        addDialogToList(new CheckKDVHarmonicDialog, "Гармоники");
        addDialogToList(new CheckKDVVibrDialog, "Вибрации");
        //            VibrDialog = new CheckDialogVibrKDV(BoardTypes::BT_BASE);
        break;
    }
    default:
        assert(false);
    }
    Module::create(std::move(JOUR));
}

void TuneModule::create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
{
    using namespace Modules;
    if ((typeB == BaseBoard::MTB_80) && (typeM == MezzanineBoard::MTM_84))
    {
        qDebug("Here is KIV");
        create(Modules::Model::KIV);
    }
}

void TuneModule::create(QTimer *updateTimer)
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
            create(static_cast<BaseBoard>(typeb), static_cast<MezzanineBoard>(typem));
            break;
        }
    }
    else
    {
        quint16 mtype = board.type();
        create(Modules::Model(mtype));
    }
    QList<UDialog *> dlgs = dialogs();
    for (auto *d : dlgs)
    {
        connect(updateTimer, &QTimer::timeout, d, &UDialog::reqUpdate);
        d->uponInterfaceSetting();
    }
}
