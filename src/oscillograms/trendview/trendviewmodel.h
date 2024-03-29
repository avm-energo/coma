#pragma once

#include <QMap>
#include <QObject>
#include <QVector>
#include <bitset>

constexpr int MAXGRAPHSPERPLOT = 18;

struct TrendData
{
    QString name;
    QString desc;
    QString color;
    bool isVisible;
};

class TrendViewModel : public QObject
{
    Q_OBJECT
public:
    TrendViewModel(int pointsnum, QObject *parent = nullptr);
    TrendViewModel() = default;
    virtual ~TrendViewModel() = default;

    // инициализация графиков
    // имена графиков контактных/аналоговых сигналов, количество точек, диапазон по оси Y для аналоговых
    bool addAnalogPoint(const QString &graphNum, double pointValue);
    void addDigitalPoint(const QString &GraphNum, int pointValue);
    bool setPointsAxis(float start, float step);
    bool digitalContains(const QString &key) const;
    bool analogContains(const QString &key) const;

    void processDigitalNames(const QStringList &list);
    void processAnalogNames(const QStringList &list);
    void toExcel();

    virtual QStringList analogColors() const = 0;
    virtual QStringList digitalColors() const
    {
        return {};
    };
    virtual QStringList analogDescriptions() const = 0;
    virtual QStringList digitalDescriptions() const
    {
        return {};
    };
    ;
    virtual QStringList analogValues() const = 0;
    virtual QStringList digitalValues() const
    {
        return {};
    };
    ;

    virtual QString xAxisDescription() const
    {
        return "Время, мс";
    };

    virtual void addDigitalValue(const QString &value)
    {
        Q_UNUSED(value)
    }

    virtual void addAnalogValue(const QString &value)
    {
        Q_UNUSED(value)
    }

    virtual std::bitset<32> analogEnableFlags() const
    {
        return { 0xffffffff };
    }

    virtual std::bitset<32> digitalEnableFlags() const
    {
        return { 0xffffffff };
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

    void setFilename(const QString &filename);

    QMap<QString, QVector<double>> m_analogMainData, m_digitalMainData;

private:
    QVector<double> m_mainPoints;
    quint32 m_idOsc;
    float m_xmax, m_xmin;
    quint32 m_length;
    QString m_filename;

    int pointsNum;
    QStringList digitalNames, analogNames;

signals:
    void recordsOverall(qint64 num);
    void currentRecord(qint64 num);
    void eventMessage(const QString &msg);
    void finished();
};
