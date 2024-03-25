#pragma once

#include "filemanager.h"
#include "trendview/trendviewdialog.h"

#include <gen/std_ext.h>
#include <gen/stdfunc.h>
#include <optional>

class OscManager : public FileManager<S2::OscHeader>
{
public:
    OscManager() = default;
    virtual ~OscManager();

    struct Kostyl
    {
        float BegFr;
        float StepFr;
        quint32 can;
        quint32 len;
    };

    void loadOsc(TrendViewModel *model);
    std::unique_ptr<TrendViewModel> load(const FileStruct &fs) const;
    std::unique_ptr<TrendViewModel> load(const Record &record, const FileStruct &fs) const;

    File::Vector loadFromFile(const QString &filename) const override;
    bool loadRecords(const DataTypes::S2FilePack &input, File::Vector &output) const override;

    Record loadCommon(const FileStruct &fs) const
    {
        Kostyl kostyl;
        assert(((fs.ID) == MT_HEAD_ID) || ((fs.ID) == MT_SPEC_ID));
        assert(fs.data.size() == sizeof(Record));
        Record record;
        if (fs.ID == MT_HEAD_ID)
            memcpy(&record, fs.data.data(), sizeof(Record));
        else
        {
            memcpy(&kostyl, fs.data.data(), sizeof(Kostyl));
            record.time = kostyl.BegFr;
            record.step = kostyl.StepFr;
            record.len = kostyl.len / 2;
        }
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
    /// FIXME: А как такое обрабатывать когда будут вынесены осциллограммы в XML?
    static inline const auto isOsc = [](const DataTypes::S2Record &record) {
        // ##TODO add other oscs
        return ((record.ID == AVTUK_85::OSC_ID) || (record.ID == AVTUK_8X::OSC_ID)          //
            || ((record.ID >= AVTUK_21::OSC_ID_MIN) && (record.ID <= AVTUK_21::OSC_ID_MAX)) //
            || (record.ID == AVTUK_KDV::OSC_ID) || (record.ID == AVTUK_KDV::OSCV_ID)        //
            || (record.ID == AVTUK_KDV::SPEC_ID) || (record.ID == AVTUK_KDV::SPECV_ID)      //
            || ((record.ID >= AVTUK_KIV::OSC_ALL_ID) && (record.ID <= AVTUK_KIV::OSC_C_ID)) //
        );
    };

private:
    std::unique_ptr<TrendViewModel> m_model;
    TrendViewDialog *trendDialog;
    std::optional<Record> oscHeader;
    const QStringList phases { "фазы А, В, С", "фаза А", "фаза В", "фаза С" };
};
