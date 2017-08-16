#ifndef REPORT_H
#define REPORT_H

#include <QString>
#include <QMap>

class Report
{
    struct ReportFromDevice
    {
        quint32 UnixTime; // Дата и время проведения испытаний (до 1 сек)
        QList<double> ParametersValues; // перечисление общих значений (не табличных)
        QList<QList<double> > Values; // перечисление табличных значений
    };

    struct ReportDetails
    {
        QString Organization; // Наименование организации
        QString DateTime; // Дата и время записи отчёта
        QMap<QString, QString> ParametersValues; // Данные испытаний в виде карты "Параметр" - "Значение" для основной (не табличной) записи
        QList<QStringList> Headers; // Наборы заголовков для таблиц (сколько QStringList в списке, столько и таблиц)
        QList<QList<QStringList> > Values; // Наборы значений для таблиц
    };

public:
    Report();


};

#endif // REPORT_H
