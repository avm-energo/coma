#include "fwuploaddialog.h"

#include "../dialogs/keypressdialog.h"
#include "../module/board.h"
#include "../s2/s2util.h"
#include "../widgets/epopup.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>
#include <QVBoxLayout>
#include <gen/colors.h>
#include <gen/error.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>
#include <gen/utils/crc32.h>
#include <map>

namespace crypto
{
static constexpr char hash[] = "fb001dfcffd1c899f3297871406242f097aecf1a5342ccf3ebcd116146188e4b";
static constexpr char name[] = "fwHash";
}

FWUploadDialog::FWUploadDialog(QWidget *parent)
    : UDialog(crypto::hash, crypto::name, parent), uploadStatus(true), parser(new S2Dev::HexParser(this))
{
    SetupUI();
    QObject::connect(parser, &S2Dev::HexParser::error, this, &FWUploadDialog::errorHandle);
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
    if (!checkPassword())
    {
        qCritical() << Error::GeneralError;
        return;
    }

    auto filepath = WDFunc::ChooseFileForOpen(this, "Program Version (*.hex)");
    if (filepath.isEmpty())
        return;

    QByteArray ba;
    Error::Msg res = Files::LoadFromFile(filepath, ba);
    if (res != Error::Msg::NoError)
    {
        qCritical() << "Ошибка файла ПО" << res;
        return;
    }

    uploadStatus = true;
    parser->parseFile(ba);
    if (!uploadStatus)
    {
        EMessageBox::error(this, errorMsg());
        return;
    }
    auto s2array = parser->getS2Format();
    if (!uploadStatus)
    {
        EMessageBox::error(this, errorMsg());
        return;
    }

    auto fileType = std_ext::to_underlying(S2::FilesEnum::Firmware);
    QByteArray firmware;
    S2Util::StoreDataMem(firmware, s2array, fileType);
    if (firmware.isEmpty())
    {
        qCritical() << Error::SizeError;
        EMessageBox::error(this, "Получен некорректный размер файла.");
        return;
    }
    setSuccessMsg("ПО записано успешно");
    BaseInterface::iface()->writeFile(fileType, firmware);
}

void FWUploadDialog::RunSoft()
{
    if (!checkPassword())
        return;
    setSuccessMsg("Переход на новое ПО выполнен успешно");
    BaseInterface::iface()->writeCommand(Commands::C_StartFirmwareUpgrade);
}

void FWUploadDialog::errorHandle(S2Dev::HexParseError error)
{
    using namespace S2Dev;
    static const std::map<HexParseError, QString> errMessages {
        { HexParseError::InvalidHexRecord, "Битый HEX-файл: некорректная запись." },                         //
        { HexParseError::InvalidRecordSize, "Битый HEX-файл: некорректный размер записи." },                 //
        { HexParseError::IncorrectChecksum, "Битый HEX-файл: контрольная сумма не совпадает с указанной." }, //
        { HexParseError::InvalidBlockAddress, "Битый HEX-файл: в файле указан некорректный адрес." }         //
    };

    auto valueIterator = errMessages.find(error);
    if (valueIterator != errMessages.cend())
        setErrorMsg(valueIterator->second);
    else
        setErrorMsg("Получен повреждённый HEX-файл.");
    uploadStatus = false;
}
