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

FWUploadDialog::FWUploadDialog(QWidget *parent) : UDialog(parent)
{
    m_password = "admin";
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void FWUploadDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    ETableView *tv = new ETableView;
    tv->setObjectName("fwp");

    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = WDFunc::NewPB(this, "", "Записать ПО в память " + tmps, this, &FWUploadDialog::LoadFW);
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, 1, 1, 1, 1);
    ;
    pb = WDFunc::NewPB(this, "", "Перейти на новое ПО", this, &FWUploadDialog::RunSoft);
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);

    glyout->addWidget(pb, 2, 1, 1, 1);

    lyout->addLayout(glyout, Qt::AlignTop);
    lyout->addWidget(tv, 89);
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
    Error::Msg res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Program Version (*.hex)"), ba);
    if (res != Error::Msg::NoError)
    {
        qCritical() << "Ошибка файла ПО" << res;
        return;
    }

    auto firmwareS2 = S2::ParseHexToS2(ba);
    if (firmwareS2.isEmpty())
        qCritical() << Error::SizeError;
    BaseInterface::iface()->writeS2File(DataTypes::Firmware, &firmwareS2);
    return;
}

void FWUploadDialog::RunSoft()
{
    if (!checkPassword())
        return;
    // FIXME After create usb
    BaseInterface::iface()->writeCommand(Queries::QC_StartFirmwareUpgrade);
    //        if (Commands::RunVPO() != Error::Msg::NoError)
    //        {
    //            WARNMSG("Ошибка перехода на новое ПО");
    //            QMessageBox::information(this, "Ошибка", "Ошибка");
    //        }
    //        else
    //        {
    //            QMessageBox::information(this, "Успешно", "Переход на новое ПО выполнен успешно");
    //        }
}

// bool FWUploadDialog::WriteCheckPassword()
//{
//    KeyPressDialog dlg; // = new KeyPressDialog;
//    return dlg.CheckPassword("admin");
//}
