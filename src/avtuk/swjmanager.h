#pragma once
#include "../gen/modules.h"
#include "../gen/std_ext.h"
#include "filemanager.h"

#include <QAbstractItemModel>
class SwjManager : public FileManager<S2DataTypes::SwitchJourRecord>
{
public:
    struct SwjModel
    {
        QAbstractItemModel *commonModel;
        QAbstractItemModel *detailModel;
    };

    SwjManager() = default;
    void loadFromFile(const QString &filename) override;

    Record loadCommon(const FileStruct &fs) const
    {
        assert(std_ext::to_underlying(fs.filenum) == AVTUK_85::SWJ_ID);
        assert(fs.filedata.size() == sizeof(Record));
        Record record;
        memcpy(&record, fs.filedata.data(), sizeof(Record));
        return record;
    }
    [[nodiscard]] SwjModel load(const DataTypes::FileStruct &fs);

private:
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

    QString craftType(quint8 id);
    QString switchType(quint32 value);
    QString commutationType(quint32 value);
    QString result(quint16 value);
    QString commutationPhases(quint32 value);
    QString floatToString(float value);
};
