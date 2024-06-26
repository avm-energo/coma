#include "dialogs/fwuploaddialog.h"

#include <QDebug>
#include <QVBoxLayout>
#include <gen/error.h>
#include <gen/files.h>
#include <map>
#include <s2/s2util.h>
#include <widgets/epopup.h>
#include <widgets/wd_func.h>

FWUploadDialog::FWUploadDialog(Device::CurrentDevice *device, QWidget *parent)
    : UDialog(device, parent), uploadStatus(FirmwareUploadStatus::Start), parser(new S2::HexParser(this))
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

    if (!EMessageBox::question(this,
            "Вы уверены, что следует записать выбранный файл?\n"
            "Полный путь до файла ВПО: "
                + filepath))
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

    constexpr auto fileType = std_ext::to_underlying(S2::FilesEnum::Firmware);
    QByteArray firmware;
    S2Util::StoreDataMem(firmware, s2array, fileType);
    if (firmware.isEmpty())
    {
        qCritical() << Error::SizeError;
        EMessageBox::error(this, "Получен файл с некорректным размером.");
        return;
    }
    engine()->currentConnection()->writeFirmware(firmware);
    uploadStatus = FirmwareUploadStatus::Written;
}

void FWUploadDialog::updateGeneralResponse(const DataTypes::GeneralResponseStruct &response)
{
    if (!updatesEnabled())
        return;

    if (response.type == DataTypes::GeneralResponseTypes::Ok)
    {
        if (uploadStatus == FirmwareUploadStatus::Written)
        {
            engine()->currentConnection()->writeCommand(Commands::C_StartFirmwareUpgrade);
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
