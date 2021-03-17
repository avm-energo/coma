#pragma once
#include <QAbstractSpinBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QWidget>
template <size_t N, typename T, class S, std::enable_if_t<std::is_base_of<QAbstractSpinBox, S>::value, bool> = true>
class BaseSpinBoxGroup : public QWidget
{
public:
    explicit BaseSpinBoxGroup(QWidget *parent = nullptr) : QWidget(parent)
    {
        QHBoxLayout *layout = new QHBoxLayout;

        for (auto i = 0; i != N; ++i)
        {
            layout->addWidget(new S(this));
        }
        setLayout(layout);
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
    template <std::enable_if_t<!std::is_same<float, T>::value, bool> = true> std::array<T, N> value() const
    {
        auto spinBoxes = findChildren<S *>();
        std::array<T, N> array;
        std::transform(
            spinBoxes.cbegin(), spinBoxes.cend(), array.begin(), [](const auto *spinBox) { return spinBox->value(); });
        return array;
    }
    std::array<float, N> value() const
    {
        auto spinBoxes = findChildren<S *>();
        std::array<float, N> array;
        std::transform(
            spinBoxes.cbegin(), spinBoxes.cend(), array.begin(), [](const auto *spinBox) { return spinBox->value(); });
        return array;
    }
    void setValue(const std::array<T, N> &array)
    {

        auto spinBoxes = findChildren<S *>();
        std::transform(spinBoxes.begin(), spinBoxes.end(), array.cbegin(), spinBoxes.begin(),
            [](auto *spinBox, const T value) { spinBox->setValue(value); });
    }
    void setValue(const std::array<float, N> &array)
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

signals:
private:
    T m_minimum;
    T m_maximum;
    T m_singleStep;
    int m_decimals;
};

template <size_t N> using DoubleSpinBoxGroup = BaseSpinBoxGroup<N, double, QDoubleSpinBox>;
template <size_t N> using SpinBoxGroup = BaseSpinBoxGroup<N, int, QSpinBox>;
