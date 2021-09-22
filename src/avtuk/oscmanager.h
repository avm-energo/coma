#pragma once
#include "../gen/modules.h"
#include "../gen/std_ext.h"
#include "../gen/stdfunc.h"
#include "filemanager.h"
#include "trendviewdialog.h"

#include <QObject>
#include <optional>

class OscManager : public FileManager<S2DataTypes::OscHeader>
{
public:
    OscManager() = default;
    void loadOscFromFile(const QString &filename);
    void loadOsc(std::unique_ptr<TrendViewModel> &&model);
    std::unique_ptr<TrendViewModel> load(const FileStruct &fs);

    void loadSwjFromFile(const QString &filename);
    void loadFromFile(const QString &filename) override;

    Record loadCommon(const FileStruct &fs) const
    {
        assert(std_ext::to_underlying(fs.filenum) == MT_HEAD_ID);
        assert(fs.filedata.size() == sizeof(Record));
        Record record;
        memcpy(&record, fs.filedata.data(), sizeof(Record));
        return record;
    }
    void setHeader(Record &&header) noexcept
    {
        oscHeader = std::move(header);
    }
    void setHeader(const Record &header) noexcept
    {
        oscHeader = header;
    }

private:
    UniquePointer<TrendViewDialog> trendDialog;
    std::optional<Record> oscHeader;
    const QStringList phases { "фазы А, В, С", "фаза А", "фаза В", "фаза С" };
};
