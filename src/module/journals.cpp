#include "journals.h"

#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"

#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QVariant>
#include <QXlsx/xlsxdocument.h>
#include <cmath>

Journals::Journals(QMap<Modules::JournalType, DataTypes::JournalDesc> &jourMap, QObject *parent)
    : QObject(parent), m_timezone(TimeFunc::userTimeZone()), m_jourMap(jourMap)
{
    m_workJourDescription = jourMap.value(Modules::JournalType::Work).desc;

    m_measJourHeaders = jourMap.value(Modules::JournalType::Meas).header;
    auto time_pos = std::find_if(
        m_measJourHeaders.begin(), m_measJourHeaders.end(), [](const QString &str) { return str.contains("UTC"); });
    if (time_pos != m_measJourHeaders.end())
        time_pos->replace("UTC", TimeFunc::userTimeZoneName());

    m_sysModel = new EDynamicTableModel(this);
    m_workModel = new EDynamicTableModel(this);
    m_measModel = new EDynamicTableModel(this);
}
void Journals::SetProxyModels(
    QSortFilterProxyModel *workmdl, QSortFilterProxyModel *sysmdl, QSortFilterProxyModel *measmdl)
{
    _proxyWorkModel = workmdl;
    _proxySysModel = sysmdl;
    _proxyMeasModel = measmdl;
}

Journals::~Journals()
{
}

void Journals::SetJourType(DataTypes::FilesEnum jourtype)
{
    m_jourType = jourtype;
}

void Journals::SetJourFile(const QString &jourfile)
{
    m_jourFile = jourfile;
}

int Journals::workJournalID()
{
    return m_jourMap.value(Modules::JournalType::Work).id;
}

QVector<QVector<QVariant>> Journals::createCommon(const QByteArray &array, const int eventid, const QStringList &desc)
{
    const auto basize = array.size();
    const char *file = array.constData();

    const auto recordsize = sizeof(AVM::EventStruct);
    QVector<QVector<QVariant>> ValueLists;
    ValueLists.reserve(basize / recordsize);

    int counter, i;
    std::vector<CommonEvent> events;
    for (i = 0, counter = 0; i < basize; i += recordsize)
    {
        AVM::EventStruct event;
        memcpy(&event, file, recordsize);
        file += recordsize;
        i += recordsize;
        if (event.Time == ULLONG_MAX)
            continue;
        ++counter;
        int N;

        memcpy(&N, &event.EvNum, sizeof(event.EvNum));
        N = (N & 0x00FFFFFF) - eventid;
        Q_ASSERT((N <= desc.size()) && (N >= 0));
        QString eventDesc;
        if ((N <= desc.size()) && (N >= 0))
        {
            eventDesc = desc.at(N);
        }
        else
            eventDesc = "Некорректный номер события";

        QString eventType;
        if (event.EvType)
            eventType = "Пришло";
        else
            eventType = "Ушло";

        CommonEvent commonEvent { counter, event.Time, eventDesc, eventType };
        events.push_back(commonEvent);
    }
    std::sort(events.begin(), events.end(),
        [](const CommonEvent &lhs, const CommonEvent &rhs) { return lhs.time > rhs.time; });
    auto timeZone = TimeFunc::userTimeZone();
    std::transform(
        events.cbegin(), events.cend(), std::back_inserter(ValueLists), [timeZone](const CommonEvent &event) {
            return QVector<QVariant> { event.counter, TimeFunc::UnixTime64ToInvStringFractional(event.time, timeZone),
                event.desc, event.direction };
        });
    return ValueLists;
}

void Journals::FillEventsTable(const QByteArray &ba)
{
    EDynamicTableModel *model;

    QStringList sl;
    int mineventid = -1;
    if (m_jourType == DataTypes::JourSys)
    {
        mineventid = AVM::sysJourId;
        sl = AVM::sysJourDescription;
        model = m_sysModel;
    }
    else
    {
        mineventid = workJournalID();
        sl = m_workJourDescription;
        model = m_workModel;
    }
    if (mineventid == -1)
        return;
    if (!model->isEmpty())
        model->clearModel();

    const QVector<QVector<QVariant>> ValueLists = createCommon(ba, mineventid, sl);

    auto header = AVM::eventJourHeaders;
    auto time_pos = std::find_if(header.begin(), header.end(), [](const QString &str) { return str.contains("UTC"); });
    if (time_pos != header.end())
        time_pos->replace("UTC", TimeFunc::userTimeZoneName());
    model->setHorizontalHeaderLabels(header);

    model->fillModel(ValueLists);
    resultReady(model);
}

void Journals::FillMeasTable(const QByteArray &ba)
{
    if (ba.isNull() || ba.isEmpty())
    {
        qWarning() << "Meas Byte Array is empty";
        return;
    }
    auto model = m_measModel;
    if (!model->isEmpty())
        model->clearModel();

    const QVector<QVector<QVariant>> ValueLists = createMeas(ba);

    Q_ASSERT(!m_measJourHeaders.isEmpty());
    model->setHorizontalHeaderLabels(m_measJourHeaders);
    if (model->columnCount() < 3)
    {
        qCritical("Column count error");
        return;
    }

    for (int i = 2; i < model->columnCount(); ++i)
        model->setColumnFormat(i, 4); // set 4 diits precision for all cells starting 2
    model->fillModel(ValueLists);
    resultReady(model);
}

void Journals::resultReady(EDynamicTableModel *model)
{
    QSortFilterProxyModel *pmdl;

    switch (m_jourType)
    {
    case DataTypes::JourWork:
        pmdl = _proxyWorkModel;
        break;
    case DataTypes::JourSys:
        pmdl = _proxySysModel;
        break;
    case DataTypes::JourMeas:
        pmdl = _proxyMeasModel;
        break;
    default:
        qDebug("Default case");
        return;
    }

    pmdl->setSourceModel(model);

    emit Done("Прочитано успешно", m_jourType);
}

void Journals::FillJour(const DataTypes::FileStruct &fs)
{
    m_jourType = DataTypes::FilesEnum(fs.ID);
    switch (fs.ID)
    {
    case DataTypes::JourMeas:
        FillMeasTable(fs.data);
        break;
    case DataTypes::JourSys:
        FillEventsTable(fs.data);
        break;
    case DataTypes::JourWork:
        FillEventsTable(fs.data);
        break;
    default:
        break;
    }
}

void Journals::saveJour(DataTypes::FilesEnum jtype, QString filename)
{
    QString jourtypestr;
    EDynamicTableModel *pmdl = nullptr;
    switch (jtype)
    {
    case DataTypes::JourSys:
        jourtypestr = "Системный журнал";
        pmdl = m_sysModel;
        break;
    case DataTypes::JourMeas:
        jourtypestr = "Журнал измерений";
        pmdl = m_measModel;
        break;
    case DataTypes::JourWork:
        jourtypestr = "Журнал событий";
        pmdl = m_workModel;
        break;
    default:
        break;
    }

    if (!pmdl)
        return;

    QXlsx::Document doc(filename);
    QXlsx::Worksheet *workSheet = doc.currentWorksheet();

    QXlsx::CellReference cellJourType(1, 1);
    QXlsx::CellReference cellModuleType(2, 1);
    QXlsx::CellReference cellDate(3, 1);
    QXlsx::CellReference cellTime(4, 1);

    workSheet->writeString(cellJourType, jourtypestr);

    workSheet->writeString(cellModuleType, "Модуль: ");
    cellModuleType.setColumn(2);

    workSheet->writeString(cellModuleType, Board::GetInstance().moduleName());
    cellModuleType.setColumn(3);
    workSheet->writeString(cellModuleType, "сер. ном. ");
    cellModuleType.setColumn(4);
    workSheet->writeString(cellModuleType, QString::number(Board::GetInstance().serialNumber(Board::BaseMezzAdd), 16));

    auto datetime = QDateTime::currentDateTimeUtc().toTimeZone(m_timezone);

    workSheet->writeString(cellDate, "Дата: ");
    cellDate.setColumn(2);
    workSheet->writeDate(cellDate, datetime.date());

    workSheet->writeString(cellTime, "Время: ");
    cellTime.setColumn(2);
    workSheet->writeTime(cellTime, datetime.time());
    cellTime.setColumn(3);
    workSheet->writeString(cellTime, TimeFunc::userTimeZoneName());
    // пишем в файл заголовки
    for (int i = 0; i < pmdl->columnCount(); ++i)
    {
        QXlsx::CellReference cellHeader(5, i + 1);
        QString tempString = pmdl->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
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
    emit resendMaxResult(pmdl->itemCount());
    for (int i = 0; i < pmdl->itemCount(); ++i)
    {
        qDebug() << i;

        QXlsx::CellReference currentCell(6 + i, 1);
        // номер события
        workSheet->writeNumeric(currentCell, pmdl->data(pmdl->index(i, 0), Qt::DisplayRole).toInt());

        currentCell.setColumn(2);
        // время события
        workSheet->writeString(currentCell, pmdl->data(pmdl->index(i, 1), Qt::DisplayRole).toString());

        for (int j = 2; j < pmdl->columnCount(); ++j)
        {

            currentCell.setColumn(1 + j);
            QVariant value = pmdl->data(pmdl->index(i, 1), Qt::DisplayRole);
            switch (value.type())
            {
            case QVariant::Type::Double:
                workSheet->writeNumeric(currentCell, pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toFloat());
                break;
            case QVariant::Type::Int:
                workSheet->writeNumeric(currentCell, pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toInt());
                break;
            default:
                workSheet->writeString(currentCell, pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toString());

                break;
            }
        }
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        emit resendResult(i);
    }

    doc.save();
    emit resendResult(pmdl->itemCount());
    emit Done("Файл создан успешно", m_jourType);
}
