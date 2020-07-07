#include <QApplication>
#include <QFile>
#include "journals.h"
#include "s2.h"
#include "timefunc.h"

Journals::Journals(QObject *parent) : QObject(parent)
{
}

Journals::~Journals()
{
}

void Journals::ReadJourFileAndProcessIt(const QString &jourfile, int jourtype)
{
    QFile file(jourfile);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit Error("Ошибка чтения файла");
        return;
    }
    QByteArray ba = file.readAll();
    ww->Stop();
    switch(JourType)
    {
    case JOURSYS:
        FillEventsTable(ba, JOURSYS);
        break;
    case JOURWORK:
        FillEventsTable(ba, JOURWORK);
        break;
    case JOURMEAS:
        FillMeasTable(ba, JOURMEAS);
        break;
    default:
        ERMSG("Incorrect jour type");
        return;
    }
}

void Journals::SetModel(ETableModel *mdl)
{
    _model = mdl;
}

ETableModel *Journals::Model()
{
    return _model;
}

void Journals::FillEventsTable(QByteArray &ba, int jourtype)
{
    QVector<QVector<QVariant>> lsl;
    QVector<QVariant> EventNum, Num, Time, Type;
    ETableModel *model = new ETableModel;
    EventStruct event;
    const QStringList sl = (jourtype == JOURSYS) ? SysJourDescription : WorkJourDescription;
    int mineventid = (jourtype == JOURSYS) ? SYSJOURID : WORKJOURID;
    int N = 0;
    int basize = ba.size();
    char *file = ba.data();
    int joursize = 0; // размер считанного буфера с информацией
    int recordsize = sizeof(EventStruct);
    int fhsize = sizeof(S2::FileHeader);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if ((jourtype == JOURSYS) || (jourtype == JOURWORK))
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
            memcpy(&N, &event.EvNum, sizeof(event.EvNum));
            N = (N & 0x00FFFFFF) - mineventid;
            if ((N <= sl.size()) && (N > 0))
            {
                --N;
                Num << sl.at(N);
            }
            else
                Num << "Некорректный номер события";
            Time << TimeFunc::UnixTime64ToInvStringFractional(event.Time);
            if(event.EvType)
                Type << "Пришло";
            else
                Type << "Ушло";
            ++counter;
            EventNum << counter;
        }
    }
    lsl.append(EventNum);
    lsl.append(Time);
    lsl.append(Num);
    lsl.append(Type);
    model->ClearModel();
    model->addColumn(" № ");
    model->addColumn("Дата/Время UTC");
    model->addColumn("Описание события");
    model->addColumn("Тип события");
    model->fillModel(lsl);
    emit ResultReady(model);
}
