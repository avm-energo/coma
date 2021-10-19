#include "fwuploaddialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>
#include <QVBoxLayout>
namespace crypto
{
static constexpr char hash[] = "fb001dfcffd1c899f3297871406242f097aecf1a5342ccf3ebcd116146188e4b";
static constexpr char name[] = "fwHash";
}
FWUploadDialog::FWUploadDialog(QWidget *parent) : UDialog(crypto::hash, crypto::name, parent)
{

    SetupUI();
}

void FWUploadDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;

    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = WDFunc::NewPB(this, "", "Записать ПО в память " + tmps, this, &FWUploadDialog::LoadFW);

    glyout->addWidget(pb, 1, 1, 1, 1);

    pb = WDFunc::NewPB(this, "", "Перейти на новое ПО", this, &FWUploadDialog::RunSoft);

    glyout->addWidget(pb, 2, 1, 1, 1);

    lyout->addLayout(glyout, Qt::AlignTop);
    setLayout(lyout);
}

void FWUploadDialog::LoadFW()
{
    QByteArray ba;

    if (!checkPassword())
    {
        qCritical() << Error::GeneralError;
        return;
    }
    Error::Msg res = Files::LoadFromFile(WDFunc::ChooseFileForOpen(this, "Program Version (*.hex)"), ba);
    if (res != Error::Msg::NoError)
    {
        qCritical() << "Ошибка файла ПО" << res;
        return;
    }

    auto firmwareS2 = S2::ParseHexToS2(ba);
    if (firmwareS2.isEmpty())
        qCritical() << Error::SizeError;
    setSuccessMsg("ПО записано успешно");
    BaseInterface::iface()->writeS2File(DataTypes::Firmware, &firmwareS2);
    return;
}

void FWUploadDialog::RunSoft()
{
    if (!checkPassword())
        return;
    setSuccessMsg("Переход на новое ПО выполнен успешно");
    BaseInterface::iface()->writeCommand(Queries::QC_StartFirmwareUpgrade);
}
