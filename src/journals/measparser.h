#pragma once

#include "../module/modulesettings.h"

#include <QTimeZone>

namespace journals
{

using MeasSettings = ModuleTypes::MeasJourList;
using Data = QVector<QVector<QVariant>>;

class MeasParser : public QObject
{
    Q_OBJECT
private:
    QTimeZone timezone;
    const quint8 *iter;
    std::size_t recordSize;
    std::size_t size;
    int timeIndex;
    QVector<QVariant> record;

    bool parseRecord(const MeasSettings &settings);
    std::size_t getRecordSize(const MeasSettings &settings);
    int findTimeIndex(const MeasSettings &settings);
    bool sortByTime(const QVector<QVariant> &lhs, const QVector<QVariant> &rhs);

public:
    explicit MeasParser(QTimeZone timeZone, QObject *parent = nullptr);
    void update(const QByteArray &ba, const MeasSettings &settings);
    const Data parse(const MeasSettings &settings);
};

}
