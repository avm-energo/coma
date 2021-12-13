#include "report.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"

//#include <LimeReport>
//#include <limereport/LimeReport>
//#include "limereport/lrreportengine.h"s
#include <LimeReport>
#include <QStandardItem>

ReportModel::ReportModel(QObject *parent) : QStandardItemModel(parent)
{
}

void ReportModel::UpdateItem(int row, int column, const QString &value)
{
    QStandardItem *item = this->item(row, column);
    item->setText(value);
    setItem(row, column, item);
}

void ReportModel::UpdateItem(int row, int column, float value, int tolerance)
{
    QStandardItem *item = this->item(row, column);
    item->setText(QString::number(value, 'f', tolerance));
    setItem(row, column, item);
}

float ReportModel::Item(int row, int column)
{
    return item(row, column)->text().toFloat();
}

void ReportModel::SetHeader(QStringList &sl)
{
    int count = 0;
    for (QStringList::Iterator it = sl.begin(); it != sl.end(); ++it)
        setHeaderData(count++, Qt::Horizontal, *it);
}

void ReportModel::SetSize(int rowcount, int columncount)
{
    setRowCount(rowcount);
    setColumnCount(columncount);
}

void ReportModel::SetModel(int rowcount, int columncount)
{
    clear();
    SetSize(rowcount, columncount);
    for (int i = 0; i < rowcount; ++i)
    {
        for (int j = 0; j < columncount; ++j)
        {
            QStandardItem *item = new QStandardItem("");
            setItem(i, j, item);
        }
    }
}

Report::Report(const QString &templatepath, QWidget *parent)
{
    Rep = new LimeReport::ReportEngine(parent);
    QString path = StdFunc::GetSystemHomeDir() + templatepath + ".lrxml";
    Rep->loadFromFile(path);
}

void Report::AddModel(const QString &modelname, QStandardItemModel *model)
{
    Rep->dataManager()->addModel(modelname, model, false);
}

void Report::SetVar(const QString &varname, const QString &varvalue)
{
    Rep->dataManager()->setReportVariable(varname, varvalue);
}

void Report::SetVar(const QString &varname, float varvalue, int tolerance)
{
    Rep->dataManager()->setReportVariable(varname, QString::number(varvalue, 'g', tolerance));
}

Error::Msg Report::Generate(const QString &filename)
{
    bool rep;
    /*    Rep->previewReport();
        Rep->designReport(); */
    rep = Rep->printToPDF(filename);
    if (!rep)
        return Error::Msg::GeneralError;
    else
        return Error::Msg::NoError;
}
