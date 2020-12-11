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
    // File_struct PV_file;
    // quint32 crc=0xFFFFFFFF;
    //    quint32 i;
    //    quint32 tmpi = 0;
    // void *Rptr = static_cast<void *>(&PV_file.Type);
    // tmpi = sizeof(PV_file.Type)+sizeof(PV_file.File.FileDatHeader);
    // S2ConfigType S2DR;

    if (!WriteCheckPassword())
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

    // PV_file.File_xxx_header.size=sizeof(PV_file.Type) +
    // sizeof(PV_file.File.FileDatHeader) + sizeof(ba) +
    // sizeof(PV_file.void_recHeader);
    auto firmwareS2 = S2::ParseHexToS2(ba);
    if (firmwareS2.isEmpty())
        qCritical() << Error::SizeError;
    //    QByteArray ba2(300000,Qt::Uninitialized);
    //    S2::ParseHexToS2()

    //    BaseInterface::iface()->writeS2File(DataTypes::Firmware, &firmwareS2);

    //    if (!ParseHexToS2(ba))
    //        qCritical() << Error::ReadError;
    return;
}

// Error::Msg Commands::WriteFile(int filenum, QVector<S2::DataRec> *data)
//{
//    QByteArray ba;
//    ba.resize(CN::Limits::MaxFileSize);
//    S2::StoreDataMem(&(ba.data()[0]), data, filenum);
//    // считываем длину файла из полученной в StoreDataMem и вычисляем количество сегментов
//    quint32 wrlength = static_cast<quint8>(ba.at(7)) * 16777216; // с 4 байта начинается FileHeader.size
//    wrlength += static_cast<quint8>(ba.at(6)) * 65536;
//    wrlength += static_cast<quint8>(ba.at(5)) * 256;
//    wrlength += static_cast<quint8>(ba.at(4));
//    wrlength += sizeof(S2::FileHeader); // sizeof(FileHeader)
//    ba.resize(wrlength);
//    EProtocom::GetInstance().SendFile(CN::Write::File, BoardTypes::BT_BASE, filenum, ba);
//    return EProtocom::GetInstance().result();
//}

void FWUploadDialog::RunSoft()
{
    if (!WriteCheckPassword())
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

bool FWUploadDialog::WriteCheckPassword()
{
    KeyPressDialog dlg; // = new KeyPressDialog;
    return dlg.CheckPassword("admin");
}

// void FWUploadDialog::WritePasswordCheck(QString psw)
//{
//    if (psw == "admin")
//        ok = true;
//    else
//        ok = false;
//    emit WritePasswordChecked();
//}

bool FWUploadDialog::ParseHexToS2(QByteArray ba)
{
    Q_UNUSED(ba)

    // FIXME After create usb
    //    if (Commands::WriteFile(3, &S2DR) != Error::Msg::NoError)
    //    {
    //        QMessageBox::information(this, "Ошибка", "Ошибка записи в модуль!");
    //        return Error::Msg::GeneralError;
    //    }
    // QMessageBox::information(this, "Успешно", "Загрузка ПО версии " + st + " прошла успешно!");
    return true;
}
