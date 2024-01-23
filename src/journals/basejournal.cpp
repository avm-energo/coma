#include "basejournal.h"

#include "../module/board.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"

#include <QApplication>
#include <QFile>
#include <gen/timefunc.h>
#include <xlsxdocument.h>

namespace journals
{

const QStringList BaseJournal::s_eventJourHeaders = {
    " № ",              //
    "Дата/Время UTC",   //
    "Описание события", //
    "Тип события",      //
    "Доп поле"          //
};

const std::map<JournalType, QString> BaseJournal::s_nameByType {
    { JournalType::System, "Системный журнал" }, //
    { JournalType::Work, "Рабочий журнал" },     //
    { JournalType::Meas, "Журнал измерений" }    //
};

BaseJournal::BaseJournal(QObject *parent)
    : QObject(parent)
    , m_timezone(TimeFunc::userTimeZone())
    , m_dataModel(new EDynamicTableModel(this))
    , m_proxyModel(new QSortFilterProxyModel(this))
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
    auto modelView = WDFunc::NewTV(parent, m_viewName, m_proxyModel.get());
    return modelView;
}

void BaseJournal::fill(const S2::S2BFile &journalFile)
{
    m_file = journalFile;
    if (!m_dataModel->isEmpty())
        m_dataModel->clearModel();
    m_dataModel->setHorizontalHeaderLabels(m_modelHeaders);
    fillModel(m_file.data);
    m_proxyModel->setSourceModel(m_dataModel.get());
    emit done("Прочитано успешно");
}

const S2::S2BFile &BaseJournal::getFile() const noexcept
{
    return m_file;
}

const QString &BaseJournal::getJournalName(const JournalType type, const QString &defaultValue) noexcept
{
    auto search = s_nameByType.find(type);
    if (search != s_nameByType.cend())
        return search->second;
    else
        return defaultValue;
}

void BaseJournal::saveToExcel(const QString &filename)
{
    // Если не удалить прошлый существующий файл, то QXlsx откроет его и
    // начнёт читать. Для больших файлов (например, журналов измерений)
    // на это тратится слишком много времени.
    if (QFile::exists(filename))
        QFile::remove(filename);

    auto doc = new QXlsx::Document(filename, this);
    auto workSheet = doc->currentWorksheet();
    QXlsx::CellReference cellJourType(1, 1);
    QXlsx::CellReference cellModuleType(2, 1);
    QXlsx::CellReference cellDate(3, 1);
    QXlsx::CellReference cellTime(4, 1);

    const quint16 type = (m_file.header.typeM << 8) | m_file.header.typeB;
    workSheet->writeString(cellJourType, getJournalName(static_cast<JournalType>(m_file.header.fname)));
    workSheet->writeString(cellModuleType, "Модуль: ");
    cellModuleType.setColumn(2);
    workSheet->writeString(cellModuleType, QString::number(type, 16));
    cellModuleType.setColumn(3);
    workSheet->writeString(cellModuleType, "сер. ном. ");
    cellModuleType.setColumn(4);
    workSheet->writeString(cellModuleType, QString::number(Board::GetInstance().serialNumber(Board::BaseMezzAdd), 16));

    auto datetime = QDateTime::fromSecsSinceEpoch(m_file.header.thetime);
    workSheet->writeString(cellDate, "Дата: ");
    cellDate.setColumn(2);
    workSheet->writeDate(cellDate, datetime.date());
    workSheet->writeString(cellTime, "Время: ");
    cellTime.setColumn(2);
    workSheet->writeTime(cellTime, datetime.time());
    cellTime.setColumn(3);
    workSheet->writeString(cellTime, TimeFunc::userTimeZoneName());

    // пишем в файл заголовки
    for (int i = 0; i < m_dataModel->columnCount(); ++i)
    {
        QXlsx::CellReference cellHeader(5, i + 1);
        QString tempString = m_dataModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        if (tempString.length() > 10)
        {
            if (!doc->setColumnWidth(cellHeader.column(), cellHeader.column(), tempString.length() * 2))
                qDebug("Couldnt change column width");
            if (tempString.contains("Описание"))
                if (!doc->setColumnWidth(cellHeader.column(), cellHeader.column(), tempString.length() * 4))
                    qDebug("Couldnt change column width");
        }
        else
        {
            if (!doc->setColumnWidth(cellHeader.column(), cellHeader.column(), 8 * sqrt(tempString.length() / 3)))
                qDebug("Couldnt change column width");
        }
        workSheet->writeString(cellHeader, tempString);
    }
    // теперь по всем строкам модели пишем данные
    auto modelSize = m_dataModel->itemCount();
    emit resendMaxResult(modelSize);
    for (int i = 0; i < modelSize; ++i)
    {
        QXlsx::CellReference currentCell(6 + i, 1);
        // номер события
        workSheet->writeNumeric(currentCell, m_dataModel->data(m_dataModel->index(i, 0), Qt::DisplayRole).toInt());
        currentCell.setColumn(2);
        // время события
        workSheet->writeString(currentCell, m_dataModel->data(m_dataModel->index(i, 1), Qt::DisplayRole).toString());
        for (int j = 2; j < m_dataModel->columnCount(); ++j)
        {
            currentCell.setColumn(1 + j);
            QVariant value = m_dataModel->data(m_dataModel->index(i, j), Qt::DisplayRole);
            switch (value.type())
            {
            case QVariant::Type::Double:
                workSheet->writeNumeric(currentCell, value.toFloat());
                break;
            case QVariant::Type::Int:
                workSheet->writeNumeric(currentCell, value.toInt());
                break;
            default:
                workSheet->writeString(currentCell, value.toString());
                break;
            }
        }
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        emit resendResult(i);
    }
    doc->save();
    doc->deleteLater();
    emit resendResult(m_dataModel->itemCount());
    emit done("Файл создан успешно");
}

}
