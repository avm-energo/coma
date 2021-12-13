#include "swjmanager.h"

#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/s2.h"
#include "oscmanager.h"

#include <QDebug>
#include <QStandardItem>
#include <QStandardItemModel>

File::Vector SwjManager::loadFromFile(const QString &filename) const
{
    QByteArray buffer;
    if (Files::LoadFromFile(filename, buffer) != Error::NoError)
        return {};

    DataTypes::S2FilePack outlist;
    S2::RestoreData(buffer, outlist);

    File::Vector vector;
    bool status = loadRecords(outlist, vector);
    if (!status)
    {
        qWarning() << Error::ReadError;
    }
    return vector;
}

bool SwjManager::loadRecords(const DataTypes::S2FilePack &input, File::Vector &output) const
{
    if (input.empty())
    {
        qWarning() << Error::SizeError << "Not enough records";
        return false;
    }

    auto foundSwj = std::find_if(input.cbegin(), input.cend(), isSwj);

    if (foundSwj == std::cend(input))
    {
        qWarning() << Error::DescError << "No swj header";
        return false;
    }

    auto model = load({ DataTypes::FilesEnum(foundSwj->ID), foundSwj->data });

    output.push_back(std::move(model));

    return true;
}

SwjModel SwjManager::load(const FileStruct &fs) const
{
    // Support only avtuk-85 swj
    assert((fs.ID) == AVTUK_85::SWJ_ID);
    auto record = loadCommon(fs);
    SwjModel model { std::make_unique<QStandardItemModel>(), std::make_unique<QStandardItemModel>() };
    auto commonModel = qobject_cast<QStandardItemModel *>(model.commonModel.get());
    auto detailModel = qobject_cast<QStandardItemModel *>(model.detailModel.get());

    commonModel->appendRow({ new QStandardItem("Номер"), new QStandardItem(QString::number(record.num)) });
    commonModel->appendRow(
        { new QStandardItem("Дата, время"), new QStandardItem(TimeFunc::UnixTime64ToString(record.time)) });
    commonModel->appendRow(
        { new QStandardItem("Аппарат"), new QStandardItem(craftType(record.typeA) + QString::number(record.numA)) });
    commonModel->appendRow({ new QStandardItem("Переключение"), new QStandardItem(switchType(record.options)) });
    commonModel->appendRow({ new QStandardItem("Тип коммутации"), new QStandardItem(commutationType(record.options)) });
    commonModel->appendRow({ new QStandardItem("Результат переключения"), new QStandardItem(result(record.result)) });

    commonModel->appendRow(
        { new QStandardItem("Коммутируемые фазы"), new QStandardItem(commutationPhases(record.options)) });

    commonModel->appendRow({ new QStandardItem("Напряжение питания цепей соленоидов, В"),
        new QStandardItem(floatToString(record.supplyVoltage)) });

    commonModel->appendRow(
        { new QStandardItem("Температура окружающей среды, Град"), new QStandardItem(floatToString(record.tOutside)) });

    detailModel->appendRow({ new QStandardItem(detailDesc.at(0)), new QStandardItem("A"), new QStandardItem("B"),
        new QStandardItem("C") });

    detailModel->appendRow({ new QStandardItem(detailDesc.at(1)),
        new QStandardItem(QString::number(record.amperage[0])), new QStandardItem(QString::number(record.amperage[1])),
        new QStandardItem(QString::number(record.amperage[2])) });

    detailModel->appendRow({ new QStandardItem(detailDesc.at(2)), new QStandardItem(QString::number(record.voltage[0])),
        new QStandardItem(QString::number(record.voltage[1])), new QStandardItem(QString::number(record.voltage[2])) });

    detailModel->appendRow({
        new QStandardItem(detailDesc.at(3)),                                         //
        new QStandardItem(QString::number(double(record.ownTime[0]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.ownTime[1]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.ownTime[2]) / 100, 'f', 2))  //
    });

    detailModel->appendRow({
        new QStandardItem(detailDesc.at(4)),                                          //
        new QStandardItem(QString::number(double(record.fullTime[0]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.fullTime[1]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.fullTime[2]) / 100, 'f', 2))  //
    });

    detailModel->appendRow({
        new QStandardItem(detailDesc.at(5)),                                         //
        new QStandardItem(QString::number(double(record.movTime[0]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.movTime[1]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.movTime[2]) / 100, 'f', 2))  //
    });

    detailModel->appendRow({
        new QStandardItem(detailDesc.at(6)),                                          //
        new QStandardItem(QString::number(double(record.archTime[0]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.archTime[1]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.archTime[2]) / 100, 'f', 2))  //
    });

    detailModel->appendRow({
        new QStandardItem(detailDesc.at(7)),                                          //
        new QStandardItem(QString::number(double(record.idleTime[0]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.idleTime[1]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.idleTime[2]) / 100, 'f', 2))  //
    });

    detailModel->appendRow({
        new QStandardItem(detailDesc.at(8)),                                            //
        new QStandardItem(QString::number(double(record.inaccuracy[0]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.inaccuracy[1]) / 100, 'f', 2)), //
        new QStandardItem(QString::number(double(record.inaccuracy[2]) / 100, 'f', 2))  //
    });

    detailModel->appendRow({
        new QStandardItem(detailDesc.at(9)),                 //
        new QStandardItem(floatToString(record.tInside[0])), //
        new QStandardItem(floatToString(record.tInside[1])), //
        new QStandardItem(floatToString(record.tInside[2]))  //
    });

    detailModel->appendRow({
        new QStandardItem(detailDesc.at(10)),             //
        new QStandardItem(floatToString(record.phyd[0])), //
        new QStandardItem(floatToString(record.phyd[1])), //
        new QStandardItem(floatToString(record.phyd[2]))  //
    });

    return model;
}

inline QString SwjManager::commutationType(quint8 value) const
{
    std::bitset<8> bitset = value;
    if (!bitset.test(2))
    {
        QString str("несинхронная от %1");
        if (bitset.test(1))
            return str.arg("АВ-ТУК");
        else
            return str.arg("внешнего устройства");
    }
    else
    {
        QString str("синхронная от %1");
        if (bitset.test(1))
            return str.arg("АВ-ТУК");
        else
            return "зарезервировано";
    }
}

inline const QString SwjManager::result(quint16 value) const
{
    return value ? "УСПЕШНО" : "НЕУСПЕШНО";
}

inline QString SwjManager::commutationPhases(quint8 value) const
{
    std::bitset<8> bitset = value;
    if (!bitset.test(3) && !bitset.test(4))
        return "фазы А, В, С (одновременно)";
    if (!bitset.test(3) && bitset.test(4))
        return "фаза A";
    if (bitset.test(3) && !bitset.test(4))
        return "фаза B";
    if (bitset.test(3) && bitset.test(4))
        return "фаза C";
    return "-";
}

inline QString SwjManager::floatToString(float value) const
{
    if (value == std::numeric_limits<float>::max())
        return "-";
    else
        return QString::number(value, 'f', 2);
}
