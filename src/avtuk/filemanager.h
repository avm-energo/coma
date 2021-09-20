#pragma once
#include "../gen/datatypes.h"
#include "../gen/std_ext.h"
#include "trendviewmodel.h"

#include <QByteArray>
#include <QVariant>
class FileManager
{
protected:
    using OscHeader = S2DataTypes::OscHeader;
    using FileStruct = DataTypes::FileStruct;

public:
    FileManager() = default;
    FileManager(const QByteArray &ba) : buffer(ba)
    {
    }
    QVariant load(const QByteArray &ba)
    {
        return {};
    };

    [[nodiscard]] std::unique_ptr<TrendViewModel> virtual load(const OscHeader &header, const FileStruct &fs) = 0;
    template <typename S2Type> [[nodiscard]] S2Type loadCommon(const FileStruct &fs) const
    {
        static_assert(false, "Unsupported type");
        return {};
    }
    template <> [[nodiscard]] OscHeader loadCommon(const FileStruct &fs) const
    {
        assert(std_ext::to_underlying(fs.filenum) == 9000);
        assert(fs.filedata.size() == sizeof(OscHeader));
        OscHeader oscHeader;
        memcpy(&oscHeader, fs.filedata.data(), sizeof(OscHeader));
        return oscHeader;
    }

protected:
    QByteArray buffer;
};
