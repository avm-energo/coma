#ifndef REPORT_H
#define REPORT_H

#include <QStandardItemModel>
#include "limereport/lrreportengine.h"

class ReportModel : public QStandardItemModel
{
    Q_OBJECT

public:
    ReportModel(QObject *parent = nullptr);
    void UpdateItem(int row, int column, const QString &value);
    void UpdateItem(int row, int column, float value, int tolerance);
    float Item(int row, int column);
    void SetHeader(QStringList &sl);
    void SetSize(int rowcount, int columncount);
    void SetModel(int rowcount, int columncount);
};

class Report
{

public:
    Report(const QString &templatepath, QWidget *parent=nullptr);

    void AddModel(const QString &modelname, QStandardItemModel *model);
    void SetVar(const QString &varname, const QString &varvalue);
    void SetVar(const QString &varname, float varvalue, int tolerance);
    void Generate(const QString &filename);

private:
    LimeReport::ReportEngine *Rep;
};

#endif // REPORT_H
