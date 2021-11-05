#include "trendviewmodel.h"

#include "../gen/colors.h"

void TrendViewModel::addAnalogPoint(const QString &graphNum, double pointValue)
{
    if (m_analogMainData.contains(graphNum))
    {
        QVector<double> tmpv = m_analogMainData.value(graphNum);
        tmpv.append(pointValue);
        m_analogMainData.insert(graphNum, tmpv);
    }
}

void TrendViewModel::addDigitalPoint(const QString &graphNum, int pointValue)
{
    if (m_digitalMainData.contains(graphNum))
    {
        QVector<double> tmpv = m_digitalMainData.value(graphNum);
        double value = static_cast<double>(pointValue) + static_cast<double>(digitalNames.indexOf(graphNum)) * 1.5;
        tmpv.append(value);
        m_digitalMainData.insert(graphNum, tmpv);
    }
}

bool TrendViewModel::setPointsAxis(float start, float step)
{
    if (step <= 0)
    {
        qWarning("Ошибка в принятой осциллограмме, шаг отрицательный или равен нулю");
        return false;
    }
    if (pointsNum == 0)
        return false;
    m_mainPoints.clear();
    for (int i = 0; i < pointsNum; ++i)
    {
        m_mainPoints << start;
        start += step;
    }
    return true;
}

int TrendViewModel::digitalContains(const QString &key) const
{
    return m_digitalMainData.contains(key);
}

int TrendViewModel::analogContains(const QString &key) const
{
    return m_analogMainData.contains(key);
}

void TrendViewModel::processDigitalNames(const QStringList &list)
{
    digitalNames = list;
    for (auto &&item : list)
        m_digitalMainData.insert(item, QVector<double>());
}

void TrendViewModel::processAnalogNames(const QStringList &list)
{
    analogNames = list;
    for (auto &&item : list)
        m_analogMainData.insert(item, QVector<double>());
}

float TrendViewModel::xmin() const
{
    return m_xmin;
}

void TrendViewModel::setXmin(float newXmin)
{
    m_xmin = newXmin;
}

float TrendViewModel::xmax() const
{
    return m_xmax;
}

void TrendViewModel::setXmax(float newXmax)
{
    m_xmax = newXmax;
}

quint32 TrendViewModel::length() const
{
    return m_length;
}

void TrendViewModel::setLength(quint32 newLength)
{
    m_length = newLength;
}

quint32 TrendViewModel::idOsc() const
{
    return m_idOsc;
}

void TrendViewModel::setIdOsc(quint32 newIdOsc)
{
    m_idOsc = newIdOsc;
}

const QVector<double> &TrendViewModel::mainPoints() const
{
    return m_mainPoints;
}

void TrendViewModel::setMainPoints(const QVector<double> &newMainPoints)
{
    m_mainPoints = newMainPoints;
}

const QMap<QString, QVector<double>> &TrendViewModel::analogMainData() const
{
    return m_analogMainData;
}

void TrendViewModel::setAnalogMainData(const QMap<QString, QVector<double>> &newAnalogMainData)
{
    m_analogMainData = newAnalogMainData;
}

const QMap<QString, QVector<double>> &TrendViewModel::digitalMainData() const
{
    return m_digitalMainData;
}

void TrendViewModel::setDigitalMainData(const QMap<QString, QVector<double>> &newDigitalMainData)
{
    m_digitalMainData = newDigitalMainData;
}

QStringList TrendViewModel85::analogColors() const
{
    return {
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR  //
    };
}

QStringList TrendViewModel85::digitalColors() const
{
    return {
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR, //
        Colors::YLWCOLOR, //
        Colors::GRNCOLOR, //
        Colors::REDCOLOR, //
        Colors::BLUCOLOR, //
        Colors::BLDCOLOR, //
        Colors::YLLCOLOR, //
        Colors::GRLCOLOR, //
        Colors::RDLCOLOR, //
        Colors::YLDCOLOR, //
        Colors::GRDCOLOR, //
        Colors::RDDCOLOR  //
    };
}

QStringList TrendViewModel85::analogDescriptions() const
{
    return {
        "Напряжение сети ф.А",        //
        "Напряжение сети ф.В",        //
        "Напряжение сети ф.С",        //
        "Ток в выключателе ф.А",      //
        "Ток в выключателе ф.В",      //
        "Ток в выключателе ф.С",      //
        "Напряжение на нагрузке ф.А", //
        "Напряжение на нагрузке ф.В", //
        "Напряжение на нагрузке ф.С"  //
    };
}

QStringList TrendViewModel85::digitalDescriptions() const
{
    return {
        "Оперативное управление включением полюса А",                                                 //
        "Оперативное управление включением полюса B",                                                 //
        "Оперативное управление включением полюса C",                                                 //
        "Оперативное управление отключением полюса А",                                                //
        "Оперативное управление отключением полюса B",                                                //
        "Оперативное управление отключением полюса C",                                                //
        "НЗ блок-контакт полюса А", "НЗ блок-контакт полюса B",                                       //
        "НЗ блок-контакт полюса C", "НО блок-контакт полюса А",                                       //
        "НО блок-контакт полюса В", "НО блок-контакт полюса С", "Контроль цепи соленоида отключения", //
        "Контроль цепи соленоида включения", "Выходная команда на включение полюса А",                //
        "Выходная команда на включение полюса В", "Выходная команда на включение полюса С",           //
        "Выходная команда на отключение полюса А", "Выходная команда на отключение полюса В",         //
        "Выходная команда на отключение полюса С"                                                     //
    };
}

QStringList TrendViewModel85::analogValues() const
{
    return { "USA", "USB", "USC", "IA", "IB", "IC", "ULA", "ULB", "ULC" };
}

QStringList TrendViewModel85::digitalValues() const
{
    return { "OCNA", "OCNB", "OCNC", "OCFA", "OCFB", "OCFC", "BKCA", "BKCB", "BKCC", "BKOA", "BKOB", "BKOC", "CSC",
        "CSO", "CNA", "CNB", "CNC", "CFA", "CFB", "CFC" };
}

QStringList TrendViewModel80::analogColors() const
{
    return { Colors::YLWCOLOR, Colors::GRNCOLOR, Colors::REDCOLOR, Colors::YLLCOLOR, Colors::GRLCOLOR, Colors::RDLCOLOR,
        Colors::YLDCOLOR, Colors::GRDCOLOR, Colors::RDDCOLOR };
}

QStringList TrendViewModel80::digitalColors() const
{
    return { Colors::YLWCOLOR, Colors::GRNCOLOR, Colors::REDCOLOR, Colors::YLLCOLOR, Colors::GRLCOLOR, Colors::RDLCOLOR,
        Colors::YLDCOLOR, Colors::GRDCOLOR, Colors::RDDCOLOR, Colors::YLWCOLOR, Colors::GRNCOLOR, Colors::REDCOLOR,
        Colors::BLUCOLOR, Colors::BLDCOLOR, Colors::YLLCOLOR, Colors::GRLCOLOR, Colors::RDLCOLOR, Colors::YLDCOLOR,
        Colors::GRDCOLOR, Colors::RDDCOLOR, Colors::YLWCOLOR, Colors::GRNCOLOR, Colors::REDCOLOR, Colors::YLLCOLOR,
        Colors::GRLCOLOR, Colors::RDLCOLOR, Colors::YLDCOLOR, Colors::GRDCOLOR, Colors::RDDCOLOR, Colors::YLWCOLOR,
        Colors::GRNCOLOR, Colors::REDCOLOR };
}

QStringList TrendViewModel80::analogDescriptions() const
{
    return {};
}

QStringList TrendViewModel80::digitalDescriptions() const
{
    return {};
}

QStringList TrendViewModel80::analogValues() const
{
    return { "USA", "USB", "USC", "IA", "IB", "IC", "ULA", "ULB", "ULC" };
}

QStringList TrendViewModel80::digitalValues() const
{
    return {};
}

QStringList TrendViewModel21::analogColors() const
{
    return { Colors::YLWCOLOR };
}

QStringList TrendViewModel21::digitalColors() const
{
    return {};
}

QStringList TrendViewModel21::analogDescriptions() const
{
    return { "Сигнал в инженерных единицах" };
}

QStringList TrendViewModel21::digitalDescriptions() const
{
    return {};
}

QStringList TrendViewModel21::digitalValues() const
{
    return {};
}
