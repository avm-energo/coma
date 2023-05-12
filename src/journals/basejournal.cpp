#include "basejournal.h"

#include "../module/board.h"
#include "../widgets/wd_func.h"

#include <QApplication>
#include <gen/timefunc.h>
#include <xlsxdocument.h>

namespace journals
{

const QStringList BaseJournal::eventJourHeaders = {
    " № ",              //
    "Дата/Время UTC",   //
    "Описание события", //
    "Тип события",      //
    "Доп поле"          //
};

BaseJournal::BaseJournal(QObject *parent)
    : QObject(parent)
    , timezone(TimeFunc::userTimeZone())
    , dataModel(new EDynamicTableModel(this))
    , proxyModel(new QSortFilterProxyModel(this))
{
}

void BaseJournal::setUserTimezone(QStringList &data)
{
    auto time_pos = std::find_if(data.begin(), data.end(), [](const QString &str) { return str.contains("UTC"); });
    if (time_pos != data.end())
        time_pos->replace("UTC", TimeFunc::userTimeZoneName());
}

ETableView *BaseJournal::createModelView(QWidget *parent) const
{
    auto modelView = WDFunc::NewTV(parent, viewName, proxyModel.get());
    return modelView;
}

const QString &BaseJournal::getName() const
{
    return jourName;
}

const QString &BaseJournal::getViewName() const
{
    return viewName;
}

const JournalType BaseJournal::getType() const
{
    return type;
}

void BaseJournal::fill(const QVariant &data)
{
    if (data.canConvert<DataTypes::FileStruct>())
    {
        auto file = data.value<DataTypes::FileStruct>();
        if (file.ID == type)
        {
            if (!dataModel->isEmpty())
                dataModel->clearModel();
            dataModel->setHorizontalHeaderLabels(headers);
            fillModel(file.data);
            proxyModel->setSourceModel(dataModel.get());
            emit done("Прочитано успешно");
        }
    }
}

void BaseJournal::save(const QString &filename)
{
    QXlsx::Document doc(filename);
    QXlsx::Worksheet *workSheet = doc.currentWorksheet();
    QXlsx::CellReference cellJourType(1, 1);
    QXlsx::CellReference cellModuleType(2, 1);
    QXlsx::CellReference cellDate(3, 1);
    QXlsx::CellReference cellTime(4, 1);

    workSheet->writeString(cellJourType, jourName);
    workSheet->writeString(cellModuleType, "Модуль: ");
    cellModuleType.setColumn(2);
    workSheet->writeString(cellModuleType, Board::GetInstance().moduleName());
    cellModuleType.setColumn(3);
    workSheet->writeString(cellModuleType, "сер. ном. ");
    cellModuleType.setColumn(4);
    workSheet->writeString(cellModuleType, QString::number(Board::GetInstance().serialNumber(Board::BaseMezzAdd), 16));

    auto datetime = QDateTime::currentDateTimeUtc().toTimeZone(timezone);
    workSheet->writeString(cellDate, "Дата: ");
    cellDate.setColumn(2);
    workSheet->writeDate(cellDate, datetime.date());
    workSheet->writeString(cellTime, "Время: ");
    cellTime.setColumn(2);
    workSheet->writeTime(cellTime, datetime.time());
    cellTime.setColumn(3);
    workSheet->writeString(cellTime, TimeFunc::userTimeZoneName());

    // пишем в файл заголовки
    for (int i = 0; i < dataModel->columnCount(); ++i)
    {
        QXlsx::CellReference cellHeader(5, i + 1);
        QString tempString = dataModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        if (tempString.length() > 10)
        {
            if (!doc.setColumnWidth(cellHeader.column(), cellHeader.column(), tempString.length() * 2))
                qDebug("Couldnt change column width");
            if (tempString.contains("Описание"))
                if (!doc.setColumnWidth(cellHeader.column(), cellHeader.column(), tempString.length() * 4))
                    qDebug("Couldnt change column width");
        }
        else
        {
            if (!doc.setColumnWidth(cellHeader.column(), cellHeader.column(), 8 * sqrt(tempString.length() / 3)))
                qDebug("Couldnt change column width");
        }
        workSheet->writeString(cellHeader, tempString);
    }
    // теперь по всем строкам модели пишем данные
    emit resendMaxResult(dataModel->itemCount());
    for (int i = 0; i < dataModel->itemCount(); ++i)
    {
        qDebug() << i;
        QXlsx::CellReference currentCell(6 + i, 1);
        // номер события
        workSheet->writeNumeric(currentCell, dataModel->data(dataModel->index(i, 0), Qt::DisplayRole).toInt());
        currentCell.setColumn(2);
        // время события
        workSheet->writeString(currentCell, dataModel->data(dataModel->index(i, 1), Qt::DisplayRole).toString());

        for (int j = 2; j < dataModel->columnCount(); ++j)
        {
            currentCell.setColumn(1 + j);
            QVariant value = dataModel->data(dataModel->index(i, 1), Qt::DisplayRole);
            switch (value.type())
            {
            case QVariant::Type::Double:
                workSheet->writeNumeric(
                    currentCell, dataModel->data(dataModel->index(i, j), Qt::DisplayRole).toFloat());
                break;
            case QVariant::Type::Int:
                workSheet->writeNumeric(currentCell, dataModel->data(dataModel->index(i, j), Qt::DisplayRole).toInt());
                break;
            default:
                workSheet->writeString(
                    currentCell, dataModel->data(dataModel->index(i, j), Qt::DisplayRole).toString());
                break;
            }
        }
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        emit resendResult(i);
    }

    doc.save();
    emit resendResult(dataModel->itemCount());
    emit done("Файл создан успешно");
}

}
