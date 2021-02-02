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
    Journals *JOUR = nullptr;
    const auto &board = Board::GetInstance();
    switch (model)
    {
    case Model::KIV:
    {
        JOUR = new JournKIV(this);
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
        JOUR = new JournKTF(this);
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
        JOUR = new JournKDV(this);
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
}
