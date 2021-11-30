#pragma once

#include <QMap>
#include <QVector>

class TrendViewModel
{
public:
    TrendViewModel(int pointsnum) : pointsNum(pointsnum)
    {
    }
    TrendViewModel() = default;
    virtual ~TrendViewModel() = default;

    // инициализация графиков
    // имена графиков контактных/аналоговых сигналов, количество точек, диапазон по оси Y для аналоговых
    void addAnalogPoint(const QString &graphNum, double pointValue);
    void addDigitalPoint(const QString &GraphNum, int pointValue);
    bool setPointsAxis(float start, float step);
    int digitalContains(const QString &key) const;
    int analogContains(const QString &key) const;

    void processDigitalNames(const QStringList &list);
    void processAnalogNames(const QStringList &list);

    virtual QStringList analogColors() const = 0;
    virtual QStringList digitalColors() const = 0;
    virtual QStringList analogDescriptions() const = 0;
    virtual QStringList digitalDescriptions() const = 0;
    virtual QStringList analogValues() const = 0;
    virtual void addAnalogValue(const QString &value)
    {
        Q_UNUSED(value)
    }
    virtual QStringList digitalValues() const = 0;
    virtual void addDigitalValue(const QString &value)
    {
        Q_UNUSED(value)
    }

    float xmin() const;
    void setXmin(float newXmin);

    float xmax() const;
    void setXmax(float newXmax);

    quint32 length() const;
    void setLength(quint32 newLength);

    quint32 idOsc() const;
    void setIdOsc(quint32 newIdOsc);

    const QVector<double> &mainPoints() const;
    void setMainPoints(const QVector<double> &newMainPoints);

    const QMap<QString, QVector<double>> &analogMainData() const;
    void setAnalogMainData(const QMap<QString, QVector<double>> &newAnalogMainData);

    const QMap<QString, QVector<double>> &digitalMainData() const;
    void setDigitalMainData(const QMap<QString, QVector<double>> &newDigitalMainData);

private:
    QMap<QString, QVector<double>> m_analogMainData, m_digitalMainData;
    QVector<double> m_mainPoints;
    quint32 m_idOsc;
    float m_xmax, m_xmin;
    quint32 m_length;

    int pointsNum;
    QStringList digitalNames, analogNames;
};

class TrendViewModel85 : public TrendViewModel
{
public:
    TrendViewModel85(int pointsnum) : TrendViewModel(pointsnum)
    {
    }
    virtual ~TrendViewModel85() override = default;

    QStringList analogColors() const override;
    QStringList digitalColors() const override;
    QStringList analogDescriptions() const override;
    QStringList digitalDescriptions() const override;
    QStringList analogValues() const override;
    QStringList digitalValues() const override;
};

class TrendViewModel80 : public TrendViewModel
{
public:
    TrendViewModel80(int pointsnum) : TrendViewModel(pointsnum)
    {
    }
    virtual ~TrendViewModel80() override = default;

    QStringList analogColors() const override;
    QStringList digitalColors() const override;
    QStringList analogDescriptions() const override;
    QStringList digitalDescriptions() const override;
    QStringList analogValues() const override;
    QStringList digitalValues() const override;
};

class TrendViewModel21 : public TrendViewModel
{
public:
    TrendViewModel21(int pointsnum) : TrendViewModel(pointsnum)
    {
    }
    virtual ~TrendViewModel21() override = default;

    QStringList analogColors() const override;
    QStringList digitalColors() const override;
    QStringList analogDescriptions() const override;
    QStringList digitalDescriptions() const override;
    QStringList analogValues() const override
    {
        return m_analogValues;
    }
    QStringList digitalValues() const override;
    void addAnalogValue(const QString &value) override
    {
        m_analogValues.append(value);
    }

private:
    QStringList m_analogValues;
};
