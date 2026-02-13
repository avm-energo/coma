#include <avm-gen/files.h>
#include <common/hex2binfileconverter.h>
#include <s2/s2util.h>
#include <avm-widgets/emessagebox.h>
#include <avm-widgets/filefunc.h>

#include <QDir>

Hex2BinFileConverter::Hex2BinFileConverter(QWidget *parent) : QObject(parent), m_parent(parent) { }

void Hex2BinFileConverter::start()
{
    m_errorHappened = false;
    auto filePath = FileFunc::chooseFileForOpen(m_parent, "*.hex");
    if (!filePath.isEmpty())
    {
        QByteArray ba = readFile(filePath);
        if (ba.isEmpty())
        {
            EMessageBox::error(m_parent, "Ошибка чтения файла");
            return;
        }
        if (EMessageBox::question(m_parent, "Преобразование в S2 (Yes) или в простой BIN (No)?"))
        {
            if (!toS2Convert(ba))
            {
                EMessageBox::error(m_parent, "Ошибка преобразования в S2");
                return;
            }
        }
        else
        {
            if (!toBinConvert(ba))
            {
                EMessageBox::error(m_parent, "Ошибка преобразования в BIN");
                return;
            }
        }
    }
    else
        EMessageBox::warning(m_parent, "Ошибка открытия файла");
}

QByteArray Hex2BinFileConverter::readFile(const QString &filepath)
{
    QByteArray ba;
    if (Files::LoadFromFile(filepath, ba) != Error::NoError)
        return {};
    return ba;
}

bool Hex2BinFileConverter::toBinConvert(QByteArray &ba)
{
    S2::HexParser *parser = new S2::HexParser(m_parent);
    QObject::connect(parser, &S2::HexParser::error, this, &Hex2BinFileConverter::errorHandler);
    parser->parseFile(ba);
    if (m_errorHappened)
        return false;
    ba = parser->getBinaryFormat();
    auto filePath = FileFunc::chooseFileForSave(m_parent, "*.bin", "bin", "output");
    if (!filePath.isEmpty())
    {
        writeToFile(filePath, ba);
        EMessageBox::information(m_parent, "Записано успешно");
        return true;
    }
    return false;
}

bool Hex2BinFileConverter::toS2Convert(QByteArray &ba)
{
    S2::HexParser *parser = new S2::HexParser(m_parent);
    QObject::connect(parser, &S2::HexParser::error, this, &Hex2BinFileConverter::errorHandler);
    parser->parseFile(ba);
    if (m_errorHappened)
        return false;
    std::vector<S2::FileStruct> S2file = parser->getS2Format();
    constexpr auto fileType = std_ext::to_underlying(S2::FilesEnum::Firmware);
    QByteArray s2ba;
    S2Util::StoreDataMem(s2ba, S2file, fileType);
    if (s2ba.isEmpty())
    {
        EMessageBox::error(m_parent, "Получен файл с некорректным размером.");
        return false;
    }
    auto filePath = FileFunc::chooseFileForSave(m_parent, "*.s2", "s2", "output");
    if (!filePath.isEmpty())
    {
        if (writeToFile(filePath, s2ba))
        {
            EMessageBox::information(m_parent, "Записано успешно");
            return true;
        }
        else
            return false;
    }
    return false;
}

bool Hex2BinFileConverter::writeToFile(const QString &filepath, const QByteArray &file)
{
    if (Files::SaveToFile(filepath, file) != Error::Msg::NoError)
    {
        EMessageBox::error(m_parent, "Ошибка записи файла");
        return false;
    }
    else
        return true;
}

void Hex2BinFileConverter::errorHandler(S2::HexParseError msg)
{
    using namespace S2;
    static const std::map<HexParseError, QString> errMessages {
        { HexParseError::InvalidHexRecord, "Битый HEX-файл: некорректная запись." },                         //
        { HexParseError::InvalidRecordSize, "Битый HEX-файл: некорректный размер записи." },                 //
        { HexParseError::IncorrectChecksum, "Битый HEX-файл: контрольная сумма не совпадает с указанной." }, //
        { HexParseError::InvalidBlockAddress, "Битый HEX-файл: в файле указан некорректный адрес." }         //
    };

    auto valueIterator = errMessages.find(msg);
    if (valueIterator != errMessages.cend())
        EMessageBox::error(m_parent, valueIterator->second);
    else
        EMessageBox::error(m_parent, "Получен повреждённый HEX-файл.");
    m_errorHappened = true;
}
