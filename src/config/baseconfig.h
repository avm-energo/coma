#pragma once
#include "../gen/s2.h"
#include "../widgets/wd_func.h"

constexpr int textColumn = 0;
constexpr int valueColumn = 1;
constexpr int extraColumn = 2;

class BaseConfig
{
public:
    BaseConfig();
    virtual ~BaseConfig() {};
    // Записать конфигурацию по умолчанию в структуру конфигурации
    virtual void SetDefConf() = 0;
    // заполнить значения полей вывода из структуры конфигурации
    virtual void Fill() = 0;
    // ввести информацию из полей вывода в конфигурацию
    virtual void FillBack() const = 0;
    // проверить конфигурацию на корректность, признаком наличия некорректностей является непустой список
    // реализовать или отказаться
    virtual void CheckConf() const {};

    inline QString nameByValue(quint32 number) const
    {
        return QString::number(number);
    }
    template <typename T> inline bool SetCBIndex(const QObject *parent, const unsigned number) const
    {
        return WDFunc::SetCBIndex(parent, nameByValue(number), S2::getRecord(number).value<T>());
    }
    template <typename T> inline bool SetSPBData(QObject *parent, const unsigned number)
    {
        return WDFunc::SetSPBData(parent, nameByValue(number), S2::getRecord(number).value<T>());
    }
    template <typename T> inline void SPBDataS2(const QObject *parent, const unsigned number) const
    {
        S2::setRecordValue({ number, WDFunc::SPBData<T>(parent, nameByValue(number)) });
    }
};
