#include "journals.h"

#include "../config/config.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"
#include "board.h"
#include "error.h"
#include "files.h"
#include "s2.h"
#include "timefunc.h"
#include "xlsxdocument.h"

//#include <QAbstractItemModelTester>
#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QVariant>

Journals::Journals(QObject *parent) : QObject(parent)
{
    _sysModel = new ETableModel;
    _workModel = new ETableModel;
    _measModel = new ETableModel;
}
void Journals::SetProxyModels(
    QSortFilterProxyModel *workmdl, QSortFilterProxyModel *sysmdl, QSortFilterProxyModel *measmdl)
{
    _proxyWorkModel = workmdl;
    _proxySysModel = sysmdl;
    _proxyMeasModel = measmdl;
    connect(_sysModel, &ETableModel::pushProgress, this, &Journals::resendResult);
    connect(_workModel, &ETableModel::pushProgress, this, &Journals::resendResult);
    connect(_measModel, &ETableModel::pushProgress, this, &Journals::resendResult);
    connect(_sysModel, &ETableModel::pushMaxProgress, this, &Journals::resendMaxResult);
    connect(_workModel, &ETableModel::pushMaxProgress, this, &Journals::resendMaxResult);
    connect(_measModel, &ETableModel::pushMaxProgress, this, &Journals::resendMaxResult);
    // auto tester = new QAbstractItemModelTester(_workModel, QAbstractItemModelTester::FailureReportingMode::Fatal,
    // this);
}

Journals::~Journals()
{
}

void Journals::SetJourType(int jourtype)
{
    _jourType = jourtype;
}

void Journals::SetJourFile(const QString &jourfile)
{
    _jourFile = jourfile;
}
/*
void Journals::SetParentWidget(QWidget *w)
{
    _parent = w;
} */

void Journals::ReadJourFileAndProcessIt()
{
    QFile file(_jourFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit Error("Ошибка чтения файла");
        return;
    }
    QByteArray ba = file.readAll();
    switch (_jourType)
    {
    case JOURSYS:
        FillEventsTable(ba);
        break;
    case JOURWORK:
        FillEventsTable(ba);
        break;
    case JOURMEAS:
        FillMeasTable(ba);
        break;
    default:
        ERMSG("Incorrect jour type");
        return;
    }
}

void Journals::FillEventsTable(QByteArray &ba)
{
    QVector<QVector<QVariant>> ValueLists;
    ETableModel *model;
    EventStruct event;
    QStringList sl;
    int mineventid = -1;
    if (_jourType == JOURSYS)
    // int joursize = 0; // размер считанного буфера с информацией
    {
        mineventid = SYSJOURID;
        sl = SysJourDescription;
        model = _sysModel;
    }
    else
    {
        switch (Board::GetInstance()->typeB())
        // switch (MTypeB)
        {
        case Config::MTB_A2:
            switch (Board::GetInstance()->typeM())
            // switch (MTypeM)
            {
            case Config::MTM_84:
                sl = WorkJourDescription;
                mineventid = WORKJOURID;
                break;
            case Config::MTM_87:
                sl = WorkJourDescriptionKTF;
                mineventid = WORKJOURIDKTF;
                break;
            }
            break;
        case Config::MTB_A3:
            break;
        }
        model = _workModel;
    }
    if (mineventid == -1)
        return;
    int N = 0;
    int basize = ba.size();
    char *file = ba.data();
    int recordsize = sizeof(EventStruct);
    int counter = 0;
    int i = 0;
    while (i < basize)
    {
        memcpy(&event, file, recordsize);
        file += recordsize;
        i += recordsize;
        if (event.Time != ULLONG_MAX)
        {
            QVector<QVariant> vl;
            ++counter;
            vl << counter;
            vl << TimeFunc::UnixTime64ToInvStringFractional(event.Time);
            memcpy(&N, &event.EvNum, sizeof(event.EvNum));
            N = (N & 0x00FFFFFF) - mineventid;
            if ((N < sl.size()) && (N > 0))
            {
                --N;
                vl << sl.at(N);
            }
            else
                vl << "Некорректный номер события";
            if (event.EvType)
                vl << "Пришло";
            else
                vl << "Ушло";
            ValueLists.append(vl);
        }
    }
    if (!model->isEmpty())
        model->clearModel();
    model->setHeaders(EventJourHeaders);
    model->fillModel(ValueLists);
    ResultReady();
}

void Journals::FillMeasTable(QByteArray &ba)
{
    if (ba.isNull() || ba.isEmpty())
    {
        qWarning() << "Meas Byte Array is empty";
        return;
    }
    qDebug() << ba.size();
    ETableModel *model = _measModel;
    QVector<QVector<QVariant>> ValueLists;
    QStringList headers;

    int recordsize;
    int basize = ba.size();
    char *file = ba.data();
    int i = 0;
    switch (Board::GetInstance()->typeB())
    {
    case Config::MTB_A2:
        switch (Board::GetInstance()->typeM())
        {
        case Config::MTM_84:
        {
            QElapsedTimer timer;
            timer.start();
            MeasureStruct meas;
            qDebug() << sizeof(MeasureStruct);
            recordsize = sizeof(MeasureStruct);
            headers = MeasJourHeaders;
            while (i < basize)
            {
                QVector<QVariant> vl;
                memcpy(&meas, file, recordsize);
                file += recordsize;
                i += recordsize;
                // MeasureStruct meas = std::move(mem);
                if (meas.Time != 0xFFFFFFFF)
                {
                    vl = { meas.NUM, TimeFunc::UnixTime32ToInvString(meas.Time), meas.Ueff[0], meas.Ueff[1],
                        meas.Ueff[2], meas.Ieff[0], meas.Ieff[1], meas.Ieff[2], meas.Frequency, meas.U0, meas.U1,
                        meas.U2, meas.I0, meas.I1, meas.I2, meas.Cbush[0], meas.Cbush[1], meas.Cbush[2], meas.Tg_d[0],
                        meas.Tg_d[1], meas.Tg_d[2], meas.dCbush[0], meas.dCbush[1], meas.dCbush[2], meas.dTg_d[0],
                        meas.dTg_d[1], meas.dTg_d[2], meas.Iunb, meas.Phy_unb, meas.Tmk, meas.Tamb };
                    if (!vl.isEmpty())
                        ValueLists.append(vl);
                }
            }
            qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds";
            break;
        }
        case Config::MTM_87:
        {
            MeasureStructKTF mem;
            recordsize = sizeof(MeasureStructKTF);
            headers = MeasJourKTFHeaders;
            while (i < basize)
            {
                QVector<QVariant> vl;
                memcpy(&mem, file, recordsize);
                file += recordsize;
                i += recordsize;
                MeasureStructKTF meas = std::move(mem);
                if (meas.Time != 0xFFFFFFFF)
                {
                    vl = { meas.NUM, TimeFunc::UnixTime32ToInvString(meas.Time), meas.Ueff[0], meas.Ueff[1],
                        meas.Ueff[2], meas.Ieff[0], meas.Ieff[1], meas.Ieff[2], meas.Frequency, meas.U0, meas.U1,
                        meas.U2, meas.I0, meas.I1, meas.I2, meas.Pf[0], meas.Pf[1], meas.Pf[2], meas.Pf[3], meas.Qf[0],
                        meas.Qf[1], meas.Qf[2], meas.Qf[3], meas.Sf[0], meas.Sf[1], meas.Sf[2], meas.Sf[3], meas.Cosphi,
                        meas.Tmk, meas.Tamb, meas.Twin };
                    if (!vl.isEmpty())
                        ValueLists.append(vl);
                }
            }
            break;
        }
        }
    case Config::MTB_A3:
        break;
    default:
        break;
    };

    if (!model->isEmpty())
        model->clearModel();
    model->setHeaders(headers);
    if (model->columnCount() < 3)
    {
        ERMSG("Column count error");
        return;
    }
    for (int i = 2; i < model->columnCount(); ++i)
        model->setColumnFormat(i, 4); // set 4 diits precision for all cells starting 2
    model->fillModel(ValueLists);
    ResultReady();
}

void Journals::ResultReady()
{
    ETableModel *mdl;
    QSortFilterProxyModel *pmdl;
    Qt::SortOrder order;

    switch (_jourType)
    {
    case Journals::JOURWORK:
        mdl = _workModel;
        pmdl = _proxyWorkModel;
        order = Qt::DescendingOrder;
        break;
    case Journals::JOURSYS:
        mdl = _sysModel;
        pmdl = _proxySysModel;
        order = Qt::DescendingOrder;
        break;
    case Journals::JOURMEAS:
        mdl = _measModel;
        pmdl = _proxyMeasModel;
        order = Qt::AscendingOrder;
        break;
    default:
        DBGMSG;
        return;
    }

    int dateidx = mdl->headerPosition("Дата/Время UTC");
    pmdl->invalidate();
    pmdl->setDynamicSortFilter(false);
    pmdl->setSourceModel(mdl);
    pmdl->sort(dateidx, order);
    emit Done("Прочитано успешно", _jourType);
}

void Journals::prepareJour(QByteArray &ba, int JourType)
{
    S2::FileHeader header;
    quint32 basize = ba.size();
    if (basize < 17)
    {
        ERMSG("basize");
    }
#ifdef __STDC_LIB_EXT1__
    memcpy_s(&header, sizeof(S2::FileHeader), ba.data(), sizeof(S2::FileHeader));
#endif
    memcpy(&header, ba.data(), sizeof(S2::FileHeader));
    if (!S2::CheckCRC32(&(ba.data())[16], (basize - 16), header.crc32))
    {
        ERMSG("CRC error");
    }
    if (header.fname != JourType)
    {
        ERMSG("Wrong filename");
    }
    int fhsize = sizeof(S2::FileHeader);
    ba.remove(0, fhsize);
    int drsize = sizeof(S2::DataRec) - sizeof(void *);
    S2::DataRec *record = reinterpret_cast<S2::DataRec *>(ba.data());
    ba.truncate(record->num_byte);
    ba.remove(0, drsize);
}

void Journals::FillSysJour(QByteArray ba)
{
    prepareJour(ba, JOURSYS);
    FillEventsTable(ba);
}

void Journals::FillMeasJour(QByteArray ba)
{
    prepareJour(ba, JOURMEAS);

    FillMeasTable(ba);
}

void Journals::FillWorkJour(QByteArray ba)
{
    prepareJour(ba, JOURWORK);
    FillEventsTable(ba);
}

void Journals::StartGetJour()
{
    switch (Board::GetInstance()->interfaceType())
    {

    case Board::InterfaceType::Ethernet:
    {

        emit ReadJour(_jourType);
        break;
    }
    case Board::InterfaceType::USB:
    {
        QByteArray ba;
        if (Commands::GetFile(_jourType, ba) == Error::Msg::NoError)
        {
            switch (_jourType)
            {
            case JOURSYS:
                FillEventsTable(ba);
                break;
            case JOURWORK:
                FillEventsTable(ba);
                break;
            case JOURMEAS:
                FillMeasTable(ba);
                break;
            default:
                break;
            }
        }
        else
            emit Error("Ошибка чтения журнала");
        break;
    }
    }
}

void Journals::StartSaveJour(int jtype, QAbstractItemModel *amdl, QString filename)
{
    QString jourtypestr;
    Qt::SortOrder order = Qt::AscendingOrder;
    switch (jtype)
    {
    case Journals::JOURSYS:
        jourtypestr = "Системный журнал";
        order = Qt::DescendingOrder;
        break;
    case Journals::JOURMEAS:
        jourtypestr = "Журнал измерений";
        order = Qt::AscendingOrder;
        break;
    case Journals::JOURWORK:
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
    workSheet->writeString(cellModuleType, ModuleBSI::GetModuleTypeString());
    cellModuleType.setColumn(3);
    workSheet->writeString(cellModuleType, "сер. ном. ");
    cellModuleType.setColumn(4);
    workSheet->writeString(cellModuleType, QString::number(ModuleBSI::SerialNum(BoardTypes::BT_MODULE), 16));

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
    emit Done("Файл создан успешно", _jourType);
}
