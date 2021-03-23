#pragma once
#include "../gen/s2.h"
#include "../module/modules.h"
#include "../widgets/wd_func.h"
constexpr int textColumn = 0;
constexpr int valueColumn = 1;
constexpr int extraColumn = 2;

namespace delegate
{
Q_NAMESPACE
enum class widgetType : int
{
    QLabel = 0,
    QDoubleSpinBox,
    DoubleSpinBoxGroup,
    IpControl,
    QCheckBox,
    CheckBoxGroup,
    QComboBox,
    QLineEdit
};
Q_ENUM_NS(widgetType)
struct Widget
{
    widgetType type;
    QString desc;
};
struct DoubleSpinBoxWidget : Widget
{
    double min;
    double max;
    int decimals;
};
struct Group
{
    int count;
};
struct DoubleSpinBoxGroup : DoubleSpinBoxWidget, Group
{
};
struct CheckBoxGroup : Widget, Group
{
};

using widgetVariant = std::variant<Widget, DoubleSpinBoxGroup, DoubleSpinBoxWidget, CheckBoxGroup>;
}
using widgetMap = std::map<BciNumber, delegate::widgetVariant>;

// helper type for the visitor
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class Module;
class WidgetFactory
{
    friend class Module;
    WidgetFactory()
    {
    }

public:
    static QWidget *createWidget(BciNumber key, QWidget *parent = nullptr);

private:
    static widgetMap widgetMap;
};

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
