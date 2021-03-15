#include "journals.h"

//#include "../config/config.h"
#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"
#include "xlsxdocument.h"

#include <cmath>

//#include <QAbstractItemModelTester>
#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QVariant>

Journals::Journals(QObject *parent) : QObject(parent)
{
    m_sysModel = new ETableModel(this);
    m_workModel = new ETableModel(this);
    _measModel = new ETableModel(this);
}
void Journals::SetProxyModels(
    QSortFilterProxyModel *workmdl, QSortFilterProxyModel *sysmdl, QSortFilterProxyModel *measmdl)
{
    _proxyWorkModel = workmdl;
    _proxySysModel = sysmdl;
    _proxyMeasModel = measmdl;
    //    connect(m_sysModel, &ETableModel::pushProgress, this, &Journals::resendResult);
    //    connect(m_workModel, &ETableModel::pushProgress, this, &Journals::resendResult);
    //    connect(_measModel, &ETableModel::pushProgress, this, &Journals::resendResult);
    //    connect(m_sysModel, &ETableModel::pushMaxProgress, this, &Journals::resendMaxResult);
    //    connect(m_workModel, &ETableModel::pushMaxProgress, this, &Journals::resendMaxResult);
    //    connect(_measModel, &ETableModel::pushMaxProgress, this, &Journals::resendMaxResult);
    // auto tester = new QAbstractItemModelTester(_workModel, QAbstractItemModelTester::FailureReportingMode::Fatal,
    // this);
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

QVector<QVariant> prepareRow(AVM::EventStruct &event, int eventID, int eventNumber, QStringList &eventDesc)
{
    int N;
    QVector<QVariant> vl { eventNumber, TimeFunc::UnixTime64ToInvStringFractional(event.Time) };

    memcpy(&N, &event.EvNum, sizeof(event.EvNum));
    N = (N & 0x00FFFFFF) - eventID;
    Q_ASSERT((N <= eventDesc.size()) && (N > 0));
    if ((N <= eventDesc.size()) && (N > 0))
    {
        vl << eventDesc.at(--N);
    }
    else
        vl << "Некорректный номер события";
    if (event.EvType)
        vl << "Пришло";
    else
        vl << "Ушло";
    return vl;
}

void Journals::FillEventsTable(const QByteArray &ba)
{
    ETableModel *model;
    AVM::EventStruct event;
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
    const auto basize = ba.size();
    const char *file = ba.constData();
    const auto recordsize = sizeof(AVM::EventStruct);
    int counter, i;
    QVector<QVector<QVariant>> ValueLists;
    ValueLists.reserve(basize / recordsize);

    for (i = 0, counter = 0; i < basize; i += recordsize)
    {
        memcpy(&event, file, recordsize);
        file += recordsize;
        i += recordsize;
        if (event.Time == ULLONG_MAX)
            continue;
        ++counter;
        QVector<QVariant> vl(prepareRow(event, mineventid, counter, sl));
        ValueLists.append(vl);
    }

    model->setHorizontalHeaderLabels(AVM::eventJourHeaders);

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
    auto model = _measModel;
    if (!model->isEmpty())
        model->clearModel();
    QVector<QVector<QVariant>> ValueLists;

    const auto basize = ba.size();
    const char *file = ba.constData();
    const auto recordsize = measureSize();
    for (int i = 0; i < basize; i += recordsize)
    {
        QVector<QVariant> vl(createMeasRecord(file));

        file += recordsize;

        if (!vl.isEmpty())
            ValueLists.append(vl);
    }

    setMeasJourHeaders();
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

void Journals::resultReady(ETableModel *model)
{
    QSortFilterProxyModel *pmdl;
    Qt::SortOrder order;

    switch (m_jourType)
    {
    case DataTypes::JourWork:
        pmdl = _proxyWorkModel;
        order = Qt::DescendingOrder;
        break;
    case DataTypes::JourSys:
        pmdl = _proxySysModel;
        order = Qt::DescendingOrder;
        break;
    case DataTypes::JourMeas:
        pmdl = _proxyMeasModel;
        order = Qt::AscendingOrder;
        break;
    default:
        qDebug("Default case");
        return;
    }

    int dateidx = model->headerPosition("Дата/Время UTC");
    pmdl->invalidate();
    pmdl->setDynamicSortFilter(false);
    pmdl->setSourceModel(model);
    pmdl->sort(dateidx, order);
    emit Done("Прочитано успешно", m_jourType);
}

void Journals::FillJour(const DataTypes::FileStruct &fs)
{
    m_jourType = fs.filenum;
    switch (fs.filenum)
    {
    case DataTypes::JourMeas:
        FillMeasTable(fs.filedata);
        break;
    case DataTypes::JourSys:
        FillEventsTable(fs.filedata);
        break;
    case DataTypes::JourWork:
        FillEventsTable(fs.filedata);
        break;
    default:
        break;
    }
}

void Journals::StartSaveJour(int jtype, QAbstractItemModel *amdl, QString filename)
{
    QString jourtypestr;
    Qt::SortOrder order = Qt::AscendingOrder;
    switch (jtype)
    {
    case DataTypes::JourSys:
        jourtypestr = "Системный журнал";
        order = Qt::DescendingOrder;
        break;
    case DataTypes::JourMeas:
        jourtypestr = "Журнал измерений";
        order = Qt::AscendingOrder;
        break;
    case DataTypes::JourWork:
        jourtypestr = "Журнал событий";
        order = Qt::DescendingOrder;
        break;
    default:
        break;
    }

    QSortFilterProxyModel *pmdl = static_cast<QSortFilterProxyModel *>(amdl);
    ETableModel *mdl = static_cast<ETableModel *>(pmdl->sourceModel());
    int dateidx = mdl->headerPosition("Дата/Время UTC");
    pmdl->sort(dateidx, order);
    QXlsx::Document *doc = new QXlsx::Document(filename);
    QXlsx::Worksheet *workSheet = doc->currentWorksheet();

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

    workSheet->writeString(cellDate, "Дата: ");
    cellDate.setColumn(2);
    workSheet->writeDate(cellDate, QDate::currentDate());

    workSheet->writeString(cellTime, "Время: ");
    cellTime.setColumn(2);
    workSheet->writeTime(cellTime, QTime::currentTime());

    // пишем в файл заголовки
    for (int i = 0; i < pmdl->columnCount(); ++i)
    {
        QXlsx::CellReference cellHeader(5, i + 1);
        QString tempString = pmdl->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
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
    emit resendMaxResult(pmdl->rowCount());
    for (int i = 0; i < pmdl->rowCount(); ++i)
    {

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
            case QMetaType::Float:
                workSheet->writeNumeric(currentCell, pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toFloat());
                break;
            case QMetaType::Int:
                workSheet->writeNumeric(currentCell, pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toInt());
                break;
            default:
                workSheet->writeString(currentCell, pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toString());
                //                if (pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toString() == "nan")
                //                    qDebug() << "Not A Number float";
                //                if (pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toString() == "inf")
                //                    qDebug() << "Infinitive float";
                break;
            }
        }
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        emit resendResult(i);
    }

    doc->save();
    emit resendResult(pmdl->rowCount());
    emit Done("Файл создан успешно", m_jourType);
}
