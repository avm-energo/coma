#pragma once

#include <QByteArray>
#include <QVariant>
#include <QVector>
#include <functional>

namespace journals
{

using Data = QVector<QVector<QVariant>>;

class FileParser : public QObject
{
    Q_OBJECT
public:
    FileParser(QObject *parent = nullptr) : QObject(parent)
    {
    }

    virtual Data parse() = 0;
};

class EventParser : public FileParser
{
public:
    EventParser(QObject *parent = nullptr) : FileParser(parent)
    {
    }
};

template <class Record> //
class MeasParser : public FileParser
{
public:
    using Iterator = Record *;
    using Checker = std::function<bool(const Record &)>;
    using Comparator = std::function<bool(const Record &, const Record &)>;
    using Transform = std::function<QVector<QVariant>(const Record &)>;

private:
    Iterator begin;
    Iterator end;
    std::size_t size;
    Checker checker;
    Comparator comparator;
    Transform transform;

public:
    MeasParser(QObject *parent = nullptr) : FileParser(parent), begin(nullptr), end(nullptr), size(0)
    {
    }

    MeasParser(const QByteArray &file, QObject *parent = nullptr) : MeasParser(parent)
    {
        init(file);
    }

    void init(const QByteArray &file)
    {
        constexpr auto recordSize = sizeof(Record);
        const auto fileSize = file.size();
        auto iter = reinterpret_cast<const Record *>(file.constData());
        size = fileSize / recordSize;
        begin = iter;
        end = iter + size;
    }

    void setup(Checker checker, Comparator comparator, Transform transform)
    {
        this->checker = checker;
        this->comparator = comparator;
        this->transform = transform;
    }

    virtual Data parse() override
    {
        Data retVal;
        retVal.reserve(size);
        QVector<Record> records;
        records.reserve(size);

        for (auto iter = begin; iter != end; iter++)
        {
            auto record = *iter;
            if (checker(record))
                records.push_back(record);
        }
        std::sort(records.begin(), records.end(), comparator);
        std::transform(records.begin(), records.end(), std::back_inserter(retVal), transform);
        return retVal;
    }
};

}
