#include "fwuploaddialog.h"

#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QVBoxLayout>
#include <gen/error.h>
#include <gen/files.h>
#include <map>
#include <s2/s2util.h>

namespace crypto
{
static constexpr char hash[] = "fb001dfcffd1c899f3297871406242f097aecf1a5342ccf3ebcd116146188e4b";
static constexpr char name[] = "fwHash";
}

FWUploadDialog::FWUploadDialog(QWidget *parent)
    : UDialog(crypto::hash, crypto::name, parent)
    , uploadStatus(FirmwareUploadStatus::Start)
    , parser(new S2::HexParser(this))
{
    QObject::connect(parser, &S2::HexParser::error, this, &FWUploadDialog::errorHandle);
    setSuccessMsg("Переход на новое ПО выполнен успешно");
    setupUI();
}

void FWUploadDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QPushButton *pb = WDFunc::NewPB(this, "", "Записать ПО в память модуля", this, &FWUploadDialog::loadFirmware);
    lyout->addWidget(pb);
    setLayout(lyout);
}

void FWUploadDialog::loadFirmware()
{
    uploadStatus = FirmwareUploadStatus::Start;
    if (!checkPassword())
    {
        uploadStatus = FirmwareUploadStatus::Error;
        qCritical() << Error::GeneralError;
        return;
    }

    auto filepath = WDFunc::ChooseFileForOpen(this, "Program Version (*.hex)");
    if (filepath.isEmpty())
        return;

    QByteArray ba;
    auto status = Files::LoadFromFile(filepath, ba);
    if (status != Error::Msg::NoError)
    {
        uploadStatus = FirmwareUploadStatus::Error;
        qCritical() << "Ошибка файла ПО" << status;
        return;
    }

    parser->parseFile(ba);
    if (uploadStatus == FirmwareUploadStatus::Error)
    {
        EMessageBox::error(this, errorMsg());
        return;
    }
    auto s2array = parser->getS2Format();
    if (uploadStatus == FirmwareUploadStatus::Error)
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
    BaseConnection::iface()->writeFile(fileType, firmware);
    uploadStatus = FirmwareUploadStatus::Written;
}

void FWUploadDialog::updateGeneralResponse(const QVariant &msg)
{
    if (!updatesEnabled())
        return;

    auto response = msg.value<DataTypes::GeneralResponseStruct>();
    if (response.type == DataTypes::GeneralResponseTypes::Ok)
    {
        if (uploadStatus == FirmwareUploadStatus::Written)
        {
            BaseConnection::iface()->writeCommand(Commands::C_StartFirmwareUpgrade);
            uploadStatus = FirmwareUploadStatus::Upgraded;
        }
        else if (uploadStatus == FirmwareUploadStatus::Upgraded)
        {
            uploadStatus = FirmwareUploadStatus::End;
            EMessageBox::information(this, successMsg());
        }
    }
    else if (response.type == DataTypes::GeneralResponseTypes::Error)
    {
        uploadStatus = FirmwareUploadStatus::Error;
        EMessageBox::error(this, Error::MsgStr[Error::Msg(response.data)]);
    }
}

void FWUploadDialog::errorHandle(S2::HexParseError error)
{
    using namespace S2;
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
    uploadStatus = FirmwareUploadStatus::Error;
}
