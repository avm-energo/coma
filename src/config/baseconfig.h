#pragma once
#include "../gen/s2.h"
#include "../widgets/wd_func.h"
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
    virtual void FillBack() = 0;
    // проверить конфигурацию на корректность, признаком наличия некорректностей является непустой список
    // реализовать или отказаться
    virtual void CheckConf() {};

    QString nameByValue(quint32 number)
    {
        return QString::number(number);
    }
    template <typename T> bool SetCBIndex(QObject *parent, unsigned number)
    {
        return WDFunc::SetCBIndex(parent, nameByValue(number), S2::getRecord(number).value<T>());
    }
    template <typename T> bool SetSPBData(QObject *parent, unsigned number)
    {
        return WDFunc::SetSPBData(parent, nameByValue(number), S2::getRecord(number).value<T>());
    }
    template <typename T> void SPBDataS2(QObject *parent, unsigned number)
    {
        S2::setRecordValue({ number, WDFunc::SPBData<T>(parent, nameByValue(number)) });
    }
};
