#include "journktf.h"
#include "../gen/timefunc.h"

JournKTF::JournKTF(QObject *parent) : Journals(parent) { }

void JournKTF::setWorkJourDescription()
{
    m_workJourDescription = QStringList() << "Отсутствует сигнал напряжения фазы A"
                                          << "Отсутствует сигнал напряжения фазы B"
                                          << "Отсутствует сигнал напряжения фазы С"
                                          << "Отсутствует сигнал тока фазы A"
                                          << "Отсутствует сигнал тока фазы B"
                                          << "Отсутствует сигнал тока фазы С"
                                          << "Перегрузка по току фазы A"
                                          << "Перегрузка по току фазы B"
                                          << "Перегрузка по току фазы C"
                                          << "Машина включена"
                                          << "Сигнализация по опасному превышению температуры обмотки"
                                          << "Неисправны все датчики температуры об-мотки"
                                          << "Аварийное сообщение по недопустимому превышению температуры обмотки"
                                          << "Сигнализация по опасному уровню пускового тока"
                                          << "Аварийное сообщение по недопустимому уровню пускового тока";
}

void JournKTF::setMeasJourHeaders()
{
    m_measJourHeaders = QStringList() << " № "
                                      << "Дата/Время UTC"
                                      << "Ueff фA"
                                      << "Ueff фB"
                                      << "Ueff фC"
                                      << "Ieff фA"
                                      << "Ieff фB"
                                      << "Ieff фC"
                                      << "Frequency"
                                      << "U0"
                                      << "U1"
                                      << "U2"
                                      << "I0"
                                      << "I1"
                                      << "I2"
                                      << "Pf фA"
                                      << "Pf фB"
                                      << "Pf фC"
                                      << "Pf сумм."
                                      << "Qf фA"
                                      << "Qf фB"
                                      << "Qf фC"
                                      << "Qf сумм."
                                      << "Sf фA"
                                      << "Sf фB"
                                      << "Sf фC"
                                      << "Sf сумм."
                                      << "CosPhi"
                                      << "Tmk,°С"
                                      << "Tamb,°С"
                                      << "Twin,°С";
}

void JournKTF::setMeasRecord(char *file, QVector<QVariant> &outvl)
{
    MeasureStruct ktfdata;
    memcpy(&ktfdata, file, sizeof(MeasureStruct));
    if (ktfdata.Time != 0xFFFFFFFF)
        outvl = { ktfdata.NUM, TimeFunc::UnixTime32ToInvString(ktfdata.Time), ktfdata.Ueff[0], ktfdata.Ueff[1],
            ktfdata.Ueff[2], ktfdata.Ieff[0], ktfdata.Ieff[1], ktfdata.Ieff[2], ktfdata.Frequency, ktfdata.U0,
            ktfdata.U1, ktfdata.U2, ktfdata.I0, ktfdata.I1, ktfdata.I2, ktfdata.Pf[0], ktfdata.Pf[1], ktfdata.Pf[2],
            ktfdata.Pf[3], ktfdata.Qf[0], ktfdata.Qf[1], ktfdata.Qf[2], ktfdata.Qf[3], ktfdata.Sf[0], ktfdata.Sf[1],
            ktfdata.Sf[2], ktfdata.Sf[3], ktfdata.Cosphi, ktfdata.Tmk, ktfdata.Tamb, ktfdata.Twin };
}

int JournKTF::measureSize() { return sizeof(MeasureStruct); }

int JournKTF::workJournalID() { return KTFWORKJOURNID; }
