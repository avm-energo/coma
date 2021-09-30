#pragma once
#include "../gen/modules.h"
#include "../gen/std_ext.h"
#include "filemanager.h"
#include "oscmanager.h"

#include <QAbstractItemModel>
class SwjManager : public FileManager<S2DataTypes::SwitchJourRecord>
{
    using SwitchJourRecord = S2DataTypes::SwitchJourRecord;

public:
    SwjManager() = default;
    virtual ~SwjManager() = default;
    File::Vector loadFromFile(const QString &filename) const override;
    bool loadRecords(const DataTypes::S2FilePack &input, File::Vector &output) const override;

    Record loadCommon(const FileStruct &fs) const
    {
        assert(std_ext::to_underlying(fs.filenum) == AVTUK_85::SWJ_ID);
        assert(fs.filedata.size() == sizeof(Record));
        Record record;
        memcpy(&record, fs.filedata.data(), sizeof(Record));
        return record;
    }
    [[nodiscard]] SwjModel load(const FileStruct &fs) const;
    bool enableOsc()
    {
        if (manager)
            return false;
        manager = std::make_unique<OscManager>();
        return true;
    }
    static inline const auto isSwj = [](const DataTypes::S2Record &record) {
        // ##TODO add other swjs
        return ((record.ID == AVTUK_85::SWJ_ID)                 //
            && (record.data.size() == sizeof(SwitchJourRecord)) //
        );
    };

    static QString craftType(quint8 id);
    static QString switchType(quint32 value);

private:
    std::unique_ptr<OscManager> manager;
    static inline const QStringList phases { "фазы А, В, С", "фаза А", "фаза В", "фаза С" };
    static inline const QStringList detailDesc {
        "Измеренное значение",                                     //
        "Действующее значение тока в момент коммутации, А",        //
        "Действующее значение напряжения в момент коммутации, кВ", //
        "Собственное время коммутации, мс",                        //
        "Полное время коммутации, мс",                             //
        "Время перемещения главного контакта, мс",                 //
        "Время горения дуги, мс",                                  //
        "Время безоперационного простоя к моменту коммутации, ч",  //
        "Погрешность синхронной коммутации, мс",                   //
        "Температура внутри привода, Град",                        //
        "Давление в гидросистеме привода, Па"                      //
    };

    QString commutationType(quint32 value) const;
    QString result(quint16 value) const;
    QString commutationPhases(quint32 value) const;
    QString floatToString(float value) const;
};