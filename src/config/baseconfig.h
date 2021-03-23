#pragma once
#include "../gen/s2.h"
#include "../widgets/wd_func.h"

constexpr int textColumn = 0;
constexpr int valueColumn = 1;
constexpr int extraColumn = 2;

namespace Delegate
{
enum class widgetType : int
{
    QDoubleSpinBox,
    DoubleSpinBoxGroup,
    IpControl,
    QCheckBox,
    CheckBoxGroup,
    QComboBox,
    QLineEdit
};

struct Widget
{
    widgetType type;
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
using widgetMap = QMap<quint32, Delegate::widgetVariant>;

// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class WidgetFactory
{
    WidgetFactory()
    {
    }

public:
    static QWidget *createWidget(quint32 key, QWidget *parent = nullptr)
    {
        QWidget *widget = nullptr;
        if (!m_widgetMap.contains(key))
            return widget;
        const auto var = m_widgetMap.value(key);
        std::visit(overloaded {
                       [&](auto arg) { qDebug("DefaultWidget"); },
                       [&](Delegate::DoubleSpinBoxGroup arg) {
                           qDebug("DoubleSpinBoxGroupWidget");
                           widget = WDFunc::NewSPBG(
                               parent, QString::number(key), arg.count, arg.min, arg.max, arg.decimals);
                       },
                       [&](Delegate::DoubleSpinBoxWidget arg) {
                           qDebug("DoubleSpinBoxWidget");
                           widget = WDFunc::NewSPB2(parent, QString::number(key), arg.min, arg.max, arg.decimals);
                       },
                       [&](Delegate::CheckBoxGroup arg) { qDebug("CheckBoxGroupWidget"); },
                   },
            var);
        return widget;
    }

private:
    static widgetMap m_widgetMap;
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
