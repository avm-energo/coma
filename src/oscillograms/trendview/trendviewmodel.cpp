#include "trendviewmodel.h"

#include "../widgets/epopup.h"

#include <QDebug>
#include <gen/comaexception.h>
#include <xlsxdocument.h>

TrendViewModel::TrendViewModel(int pointsnum, QObject *parent) : pointsNum(pointsnum), QObject(parent)
{
}

bool TrendViewModel::addAnalogPoint(const QString &graphNum, double pointValue)
{
    if (m_analogMainData.contains(graphNum))
    {
        QVector<double> tmpv = m_analogMainData.value(graphNum);
        tmpv.append(pointValue);
        m_analogMainData.insert(graphNum, tmpv);
        return true;
    }
    else
    {
        qWarning() << "Graph with name " << graphNum << " not found!";
        return false;
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

bool TrendViewModel::digitalContains(const QString &key) const
{
    return m_digitalMainData.contains(key);
}

bool TrendViewModel::analogContains(const QString &key) const
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

void TrendViewModel::toExcel()
{
    constexpr int c_div = 1000;
    //    emit eventMessage("Разборка осциллограммы");
    auto doc = new QXlsx::Document(m_filename, this);
    auto workSheet = doc->currentWorksheet();
    workSheet->writeString(2, 2, "Тип осциллограммы:");
    workSheet->writeString(2, 3, QString::number(m_idOsc));
    int currentRow = 4;
    workSheet->writeString(currentRow, 1, xAxisDescription());
    for (int i = 0; i < analogDescriptions().size(); ++i)
        workSheet->writeString(currentRow, i + 2, analogDescriptions().at(i));
    ++currentRow;
    int pushRow = currentRow;
    if (m_mainPoints.size() < 2)
    {
        emit finished();
        //        throw ComaException("Недостаточно данных по оси абсцисс");
    }
    //    else
    //    {
    //        throw ComaException("Test exception");
    //        emit finished();
    //    }
    //    emit eventMessage("Экспорт абсцисс");
    // emit recordsOverall(m_mainPoints.size());
    int curCount = 0;
    for (auto &point : m_mainPoints)
    {
        workSheet->writeNumeric(currentRow++, 1, point);
        if (curCount >= c_div)
            curCount = 0;
        ++curCount;
    }

    if (analogDescriptions().size() != analogValues().size())
    {
        emit finished();
        //        throw ComaException("Размерности массивов значений и описаний не совпадают");
    }
    int currentColumn = 2;
    //    emit eventMessage("Экспорт ординат");
    for (int j = 0; j < analogDescriptions().size(); ++j)
    {
        currentRow = pushRow;
        if (!m_analogMainData.contains(analogValues().at(j)))
        {
            emit finished();
            //            throw ComaException("Количество значений не соответствует описанию");
        }
        auto analogVals = m_analogMainData[analogValues().at(j)];
        // emit recordsOverall(vect.size());
        int count = 0;
        for (auto &analogVal : analogVals)
        {
            workSheet->writeNumeric(currentRow++, currentColumn, analogVal);
            if (count >= c_div)
            {
                // emit currentRecord(i);
                count = 0;
            }
            ++count;
        }
        ++currentColumn;
    }
    //    emit eventMessage("Запись файла");
    doc->save();
    //    emit finished();
    doc->deleteLater();
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

void TrendViewModel::setFilename(const QString &filename)
{
    m_filename = filename;
}
