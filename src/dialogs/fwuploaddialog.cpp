#include "fwuploaddialog.h"

#include "../dialogs/keypressdialog.h"
#include "../module/board.h"
#include "../s2/crc32.h"
#include "../s2/s2.h"
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

bool compare(const S2DataTypes::DataRec &lhs, const DataTypes::FileStruct &rhs)
{
    if (lhs.header.numByte == rhs.data.size())
    {
        auto lhsPtr = reinterpret_cast<const char *>(lhs.thedata);
        for (auto i = 0; i < lhs.header.numByte; i++)
        {
            if (rhs.data.data()[i] != lhsPtr[i])
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool compare(const QByteArray &lhs, const QByteArray &rhs)
{
    if (lhs.size() == rhs.size())
    {
        auto lhsPtr = lhs.data();
        auto rhsPtr = rhs.data();
        for (auto i = 0; i < lhs.size(); i++)
        {
            if (lhsPtr[i] != rhsPtr[i])
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

void FWUploadDialog::LoadFW()
{
    if (!checkPassword())
    {
        qCritical() << Error::GeneralError;
        return;
    }

    QByteArray ba;
    Error::Msg res = Files::LoadFromFile(WDFunc::ChooseFileForOpen(this, "Program Version (*.hex)"), ba);
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

    auto value = parser->getS2Format();
    if (!uploadStatus)
    {
        EMessageBox::error(this, errorMsg());
        return;
    }
    QByteArray record1;
    S2::StoreDataMem(record1, value, DataTypes::Firmware);

    //    S2Dev::CRC32 crc1;
    //    for (auto &rec : value)
    //    {
    //        auto transform = rec.serialize();
    //        // crc1.update(reinterpret_cast<const quint8 *>(&transform.header.id), sizeof(transform.header.id));
    //        crc1.update(reinterpret_cast<const quint8 *>(transform.thedata), transform.header.numByte);
    //    }

    auto firmwareS2 = S2::ParseHexToS2(ba);
    if (firmwareS2.isEmpty())
        qCritical() << Error::SizeError;
    QByteArray record2;
    S2::StoreDataMem(record2, firmwareS2, DataTypes::Firmware);

    //    S2Dev::CRC32 crc2;
    //    for (auto &rec : firmwareS2)
    //    {
    //        // crc1.update(reinterpret_cast<const quint8 *>(&rec.header.id), sizeof(rec.header.id));
    //        crc2.update(reinterpret_cast<const quint8 *>(rec.thedata), rec.header.numByte);
    //    }

    //    bool status = (crc1 == crc2);
    //    if (status)
    //    {
    //        QByteArray record2;
    //        S2::StoreDataMem(record2, firmwareS2, DataTypes::Firmware);
    //    }
    //    else
    //    {
    //        QByteArray record1;
    //        S2::StoreDataMem(record1, value, DataTypes::Firmware);
    //    }

    //    auto status = false;
    //    for (auto i = 0; i < 3; i++)
    //    {
    //        status = compare(firmwareS2[i], value[i]);
    //    }

    auto status = (record1 == record2);
    status = compare(record1, record2);

    setSuccessMsg("ПО записано успешно");
    BaseInterface::iface()->writeS2File(DataTypes::Firmware, &firmwareS2);
}

void FWUploadDialog::RunSoft()
{
    if (!checkPassword())
        return;
    setSuccessMsg("Переход на новое ПО выполнен успешно");
    BaseInterface::iface()->writeCommand(Queries::QC_StartFirmwareUpgrade);
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
