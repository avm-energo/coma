#pragma once

#include "flowlayout.h"
#include "helper.h"

#include <QAbstractSpinBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QWidget>
template <typename T, class S, std::enable_if_t<std::is_base_of<QAbstractSpinBox, S>::value, bool> = true>
class BaseSpinBoxGroup : public QWidget
{
private:
    [[nodiscard]] QWidget *createWidget(const QString &str)
    {
        QWidget *widget = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(new QLabel(str, this), 0, Qt::AlignRight);
        layout->addWidget(new S(this));
        widget->setLayout(layout);
        return widget;
    }

public:
    explicit BaseSpinBoxGroup(int count, QWidget *parent = nullptr) : QWidget(parent), m_count(count)
    {
        FlowLayout *flowLayout = new FlowLayout;
        for (auto i = 0; i != m_count; ++i)
        {
            flowLayout->addWidget(createWidget(QString::number(i + 1)));
        }
        setLayout(flowLayout);
    }
    explicit BaseSpinBoxGroup(const QStringList &list, QWidget *parent = nullptr)
        : QWidget(parent), m_count(list.size())
    {
        FlowLayout *flowLayout = new FlowLayout;
        for (auto i = 0; i != m_count; ++i)
        {
            flowLayout->addWidget(createWidget(list.at(i)));
        }
        setLayout(flowLayout);
    }
    T minimum() const
    {
        return m_minimum;
    }
    void setMinimum(T minimum)
    {
        m_minimum = minimum;
        auto spinBoxes = findChildren<S *>();
        for (auto *spinBox : spinBoxes)
            spinBox->setMinimum(m_minimum);
    }

    T maximum() const
    {
        return m_maximum;
    }
    void setMaximum(T maximum)
    {
        m_maximum = maximum;
        auto spinBoxes = findChildren<S *>();
        for (auto *spinBox : spinBoxes)
            spinBox->setMaximum(m_maximum);
    }

    T singleStep() const
    {
        return m_singleStep;
    }
    void setSingleStep(T singleStep)
    {
        m_singleStep = singleStep;
        auto spinBoxes = findChildren<S *>();
        for (auto *spinBox : spinBoxes)
            spinBox->setSingleStep(m_singleStep);
    }

    auto value() const
    {
        auto spinBoxes = findChildren<S *>();
        std::vector<T> vector;
        std::transform(spinBoxes.cbegin(), spinBoxes.cend(), std::back_inserter(vector),
            [](const auto *spinBox) { return T(spinBox->value()); });
        return vector;
    }
    void setValue(const std::vector<T> &array)
    {

        auto spinBoxes = findChildren<S *>();
        std::transform(spinBoxes.begin(), spinBoxes.end(), array.cbegin(), spinBoxes.begin(),
            [](auto *spinBox, const T value) { spinBox->setValue(value); });
    }
    void setValue(const std::vector<float> &array)
    {
        auto spinBoxes = findChildren<S *>();
        int i = 0;
        for (auto *spinBox : spinBoxes)
        {
            spinBox->setValue(array.at(i));
            ++i;
        }
    }
    int decimals() const
    {
        return m_decimals;
    }
    void setDecimals(int decimals)
    {
        m_decimals = decimals;
        auto spinBoxes = findChildren<S *>();
        for (auto *spinBox : spinBoxes)
            spinBox->setDecimals(m_decimals);
    }

    int count() const
    {
        return m_count;
    }

signals:
private:
    int m_count;
    T m_minimum;
    T m_maximum;
    T m_singleStep;
    int m_decimals;
};

using DoubleSpinBoxGroup = BaseSpinBoxGroup<double, QDoubleSpinBox>;
using SpinBoxGroup = BaseSpinBoxGroup<int, QSpinBox>;
