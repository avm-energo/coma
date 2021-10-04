#pragma once
#include "../gen/modules.h"
#include "../gen/std_ext.h"
#include "../gen/stdfunc.h"
#include "filemanager.h"
#include "trendviewdialog.h"

#include <optional>

class OscManager : public FileManager<S2DataTypes::OscHeader>
{
public:
    OscManager() = default;
    virtual ~OscManager();

    void loadOsc(TrendViewModel *model);
    std::unique_ptr<TrendViewModel> load(const FileStruct &fs) const;
    std::unique_ptr<TrendViewModel> load(const Record &record, const FileStruct &fs) const;

    File::Vector loadFromFile(const QString &filename) const override;
    bool loadRecords(const DataTypes::S2FilePack &input, File::Vector &output) const override;

    Record loadCommon(const FileStruct &fs) const
    {
        assert((fs.ID) == MT_HEAD_ID);
        assert(fs.data.size() == sizeof(Record));
        Record record;
        memcpy(&record, fs.data.data(), sizeof(Record));
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
    Record header() const
    {
        return oscHeader.value();
    }

protected:
    static inline const auto isOsc = [](const DataTypes::S2Record &record) {
        // ##TODO add other oscs
        return ((record.ID == AVTUK_85::OSC_ID)                                             //
            || (record.ID == AVTUK_8X::OSC_ID)                                              //
            || ((record.ID >= AVTUK_21::OSC_ID_MIN) && (record.ID <= AVTUK_21::OSC_ID_MAX)) //
        );
    };

private:
    std::unique_ptr<TrendViewModel> model;
    UniquePointer<TrendViewDialog> trendDialog;
    std::optional<Record> oscHeader;
    const QStringList phases { "фазы А, В, С", "фаза А", "фаза В", "фаза С" };
};
