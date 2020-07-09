#include <QApplication>
#include <QFile>
#include <QDate>
#include <QObject>
#include "QtXlsx/xlsxdocument.h"

#include "journals.h"
#include "s2.h"
#include "timefunc.h"
#include "error.h"
#include "maindef.h"
#include "commands.h"
#include "files.h"
#include "../widgets/wd_func.h"

Journals::Journals(QObject *parent) : QObject(parent)
{
}

Journals::~Journals()
{
}

void Journals::SetProxyModels(QSortFilterProxyModel *workmdl, QSortFilterProxyModel *sysmdl, QSortFilterProxyModel *measmdl)
{
    _proxyWorkModel = workmdl;
    _proxySysModel = sysmdl;
    _proxyMeasModel = measmdl;
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
    switch(_jourType)
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
    ETableModel *model = new ETableModel;
    EventStruct event;
    const QStringList sl = (_jourType == JOURSYS) ? SysJourDescription : WorkJourDescription;
    int mineventid = (_jourType == JOURSYS) ? SYSJOURID : WORKJOURID;
    int N = 0;
    int basize = ba.size();
    char *file = ba.data();
    int joursize = 0; // размер считанного буфера с информацией
    int recordsize = sizeof(EventStruct);
    int fhsize = sizeof(S2::FileHeader);

    if ((_jourType == JOURSYS) || (_jourType == JOURWORK))
    {
        file += fhsize;
        S2::DataRec jour;
        int drsize = sizeof(S2::DataRec) - sizeof(void *);
        memcpy(&jour, file, drsize);
        joursize = jour.num_byte;
        file += drsize; // move file pointer to thedata
    }
    int counter = 0;
    int i = 0;
    while (i < basize)
    {
        memcpy(&event, file, recordsize);
        file += recordsize;
        i += recordsize;
        if(event.Time != 0xFFFFFFFFFFFFFFFF)
        {
            QVector<QVariant> vl;
            ++counter;
            vl << counter;
            vl << TimeFunc::UnixTime64ToInvStringFractional(event.Time);
            memcpy(&N, &event.EvNum, sizeof(event.EvNum));
            N = (N & 0x00FFFFFF) - mineventid;
            if ((N <= sl.size()) && (N > 0))
            {
                --N;
                vl << sl.at(N);
            }
            else
                vl << "Некорректный номер события";
            if(event.EvType)
                vl << "Пришло";
            else
                vl << "Ушло";
            ValueLists.append(vl);
        }
    }
    model->ClearModel();
    model->SetHeaders(EventJourHeaders);
    model->fillModel(ValueLists);
    ResultReady(model);
}

void Journals::FillMeasTable(QByteArray &ba)
{
    ETableModel *model = new ETableModel;
    QVector<QVector<QVariant>> ValueLists;
    MeasureStruct meas;
    int recordsize = sizeof(MeasureStruct);
    int basize = ba.size();
    char *file = ba.data();
    int joursize = 0; // размер считанного буфера с информацией
    int fhsize = sizeof(S2::FileHeader);

    if (_jourType == Journals::JOURMEAS)
    {
        file += fhsize;
        S2::DataRec jour;
        int drsize = sizeof(S2::DataRec) - sizeof(void *);
        memcpy(&jour, file, drsize);
        joursize = jour.num_byte;
        file += drsize; // move file pointer to thedata
    }
    int i = 0;
    while (i < basize)
    {
        memcpy(&meas, file, recordsize);
        file += recordsize;
        i += recordsize;

        if(meas.Time != 0xFFFFFFFF)
        {
            QVector<QVariant> vl;
            vl << meas.NUM << TimeFunc::UnixTime32ToInvString(meas.Time) << meas.Ueff[0] << meas.Ueff[1] << meas.Ueff[2] << meas.Ieff[0] << meas.Ieff[1] << meas.Ieff[2] <<
                  meas.Frequency << meas.U0 << meas.U1 << meas.U2 << meas.I0 << meas.I1 << meas.I2 << meas.Cbush[0] << meas.Cbush[1] << meas.Cbush[2] <<
                  meas.Tg_d[0] << meas.Tg_d[1] << meas.Tg_d[2] << meas.dCbush[0] << meas.dCbush[1] << meas.dCbush[2] << meas.dTg_d[0] << meas.dTg_d[1] << meas.dTg_d[2] <<
                  meas.Iunb << meas.Phy_unb << meas.Tmk << meas.Tamb;
            ValueLists.append(vl);
        }
    }

   model->ClearModel();
   model->SetHeaders(MeasJourHeaders);
   if (model->columnCount() < 3)
   {
       ERMSG("Column count error");
       return;
   }
   for (int i=2; i<model->columnCount(); ++i)
       model->SetColumnFormat(i, 4); // set 4 diits precision for all cells starting 2
   model->fillModel(ValueLists);
   ResultReady(model);
}

void Journals::ResultReady(ETableModel *mdl)
{
    QSortFilterProxyModel *pmdl;
    Qt::SortOrder order;
    int dateidx = mdl->Headers().indexOf("Дата/Время UTC");
    switch(_jourType)
    {
    case Journals::JOURWORK:
        pmdl = _proxyWorkModel;
        order = Qt::DescendingOrder;
/*        WDFunc::SetTVModel(_parent, "work", _proxyModel, true);
        WDFunc::SortTV(_parent, "work", dateidx, Qt::DescendingOrder); */
        break;
    case Journals::JOURSYS:
        pmdl = _proxySysModel;
        order = Qt::DescendingOrder;
/*        WDFunc::SetTVModel(_parent, "system", _proxyModel, true);
        WDFunc::SortTV(_parent, "system", dateidx, Qt::DescendingOrder); */
        break;
    case Journals::JOURMEAS:
        pmdl = _proxyMeasModel;
        order = Qt::AscendingOrder;
/*        WDFunc::SetTVModel(_parent, "meas", _proxyModel, true);
        WDFunc::SortTV(_parent, "meas", dateidx, Qt::AscendingOrder); */
        break;
    }
    pmdl->setDynamicSortFilter(true);
    pmdl->setSourceModel(mdl);
    pmdl->sort(dateidx, order);
    emit Done("Прочитано успешно");
}

void Journals::FillSysJour(QByteArray ba)
{
    FillEventsTable(ba);
}

void Journals::FillMeasJour(QByteArray ba)
{
    quint32 crctocheck;
    quint32 basize = ba.size();
    if (basize < 17)
    {
        ERMSG("basize");
    }
    memcpy(&crctocheck, &(ba.data())[8], sizeof(quint32));
    if (!S2::CheckCRC32(&(ba.data())[16], (basize-16), crctocheck))
    {
        ERMSG("CRC error");
    }
    FillMeasTable(ba);
}

void Journals::FillWorkJour(QByteArray ba)
{
    FillEventsTable(ba);
}

void Journals::StartGetJour()
{
    char jnum = _jourType + 4;
    if (MainInterface == I_ETHERNET)
    {
        emit ReadJour(jnum);
    }
    else if (MainInterface == I_USB)
    {
        QByteArray ba;
        if(Commands::GetFile(jnum, ba) == NOERROR)
        {
            switch(_jourType)
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
    }
}

void Journals::StartSaveJour(int jtype, QAbstractItemModel *amdl, QString filename)
{
    QString jourtypestr;
    QXlsx::Format cellformat;
    Qt::SortOrder order = Qt::AscendingOrder;
    switch(jtype)
    {
    case Journals::JOURSYS:
        jourtypestr = "Системный журнал";
        order = Qt::DescendingOrder;
        cellformat.setNumberFormat("@");
        break;
    case Journals::JOURMEAS:
        jourtypestr = "Журнал измерений";
        order = Qt::AscendingOrder;
        cellformat.setNumberFormat("#.####");
        break;
    case Journals::JOURWORK:
        jourtypestr = "Журнал событий";
        order = Qt::DescendingOrder;
        cellformat.setNumberFormat("@");
        break;
    default:
        break;
    }

    QSortFilterProxyModel *pmdl = reinterpret_cast<QSortFilterProxyModel *>(amdl);
    ETableModel *mdl = reinterpret_cast<ETableModel *>(pmdl->sourceModel());
    int dateidx = mdl->Headers().indexOf("Дата/Время UTC");
    pmdl->sort(dateidx, order);
    QXlsx::Document *xlsx = new QXlsx::Document(filename);
    xlsx->write(1,1,QVariant(jourtypestr));
    xlsx->write(2,1,QVariant("Модуль: " + ModuleBSI::GetModuleTypeString() + " сер. ном. " + \
                             QString::number(ModuleBSI::SerialNum(BoardTypes::BT_MODULE), 10)));
    xlsx->write(3,1,QVariant("Дата сохранения журнала: "+QDateTime::currentDateTime().toString("dd-MM-yyyy")));
    xlsx->write(4,1,QVariant("Время сохранения журнала: "+QDateTime::currentDateTime().toString("hh:mm:ss")));

    // пишем в файл заголовки
    for (int i=0; i<pmdl->columnCount(); ++i)
        xlsx->write(5, (i+1), pmdl->headerData(i, Qt::Horizontal, Qt::DisplayRole));

    // теперь по всем строкам модели пишем данные
    for (int i=0; i<pmdl->rowCount(); ++i)
    {
        // номер события
        xlsx->write((6+i), 1, pmdl->data(pmdl->index(i, 0), Qt::DisplayRole).toString());
        // время события
        xlsx->write((6+i), 2, pmdl->data(pmdl->index(i, 1), Qt::DisplayRole).toString());
        for (int j=2; j<pmdl->columnCount(); ++j)
        {
/*            float number = ;
            QString str = QString::number(number, 'f', 4);
            str.replace('.', ','); */
            xlsx->write((6+i), (1+j), pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toFloat(), cellformat);
        }
    }
    xlsx->save();
    emit Done("Файл создан успешно");
}
