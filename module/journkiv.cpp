#include "journkiv.h"
#include "../gen/timefunc.h"

JournKIV::JournKIV(QObject *parent) : Journals(parent) { }

void JournKIV::setWorkJourDescription()
{
    m_workJourDescription = QStringList() << "Отсутствует сигнал напряжения фазы A"
                                          << "Отсутствует сигнал напряжения фазы B"
                                          << "Отсутствует сигнал напряжения фазы С"
                                          << "Нет реактивного тока канала А (ток меньше 2мА)"
                                          << "Нет реактивного тока канала B (ток меньше 2мА)"
                                          << "Нет реактивного тока канала C (ток меньше 2мА)"
                                          << "Не заданы начальные значения"
                                          << "Низкое напряжение фазы A"
                                          << "Низкое напряжение фазы B"
                                          << "Низкое напряжение фазы C"
                                          << "Сигнализация по приращению тангенса дельта ввода фазы А"
                                          << "Сигнализация по приращению тангенса дельта ввода фазы B"
                                          << "Сигнализация по приращению тангенса дельта ввода фазы C"
                                          << "Авария по приращению тангенса дельта ввода фазы А"
                                          << "Авария по приращению тангенса дельта ввода фазы B"
                                          << "Авария по приращению тангенса дельта ввода фазы C"
                                          << "Сигнализация по приращению C ввода фазы А"
                                          << "Сигнализация по приращению C ввода фазы B"
                                          << "Сигнализация по приращению C ввода фазы C"
                                          << "Авария по приращению C ввода фазы А"
                                          << "Авария по приращению C ввода фазы B"
                                          << "Авария по приращению C ввода фазы C"
                                          << "Не заданы паспортные значения"
                                          << "Сигнализация по изменению небаланса тока"
                                          << "Авария по изменению небаланса тока";
}

void JournKIV::setMeasJourHeaders()
{
    m_measJourHeaders = QStringList() << " № "
                                      << "Дата/Время UTC"
                                      << "Ueff фA"
                                      << "Ueff фB"
                                      << "Ueff фC"
                                      << "Ieff фA"
                                      << "Ieff фB"
                                      << "Ieff фC"
                                      << "Freq"
                                      << "U0"
                                      << "U1"
                                      << "U2"
                                      << "I0"
                                      << "I1"
                                      << "I2"
                                      << "Cbush фA"
                                      << "Cbush фB"
                                      << "Cbush фC"
                                      << "Tg_d фA"
                                      << "Tg_d фB"
                                      << "Tg_d фC"
                                      << "dCbush фA"
                                      << "dCbush фB"
                                      << "dCbush фC"
                                      << "dTg_d фA"
                                      << "dTg_d фB"
                                      << "dTg_d фC"
                                      << "Iunb"
                                      << "Phy_unb"
                                      << "Tmk, °С"
                                      << "Tamb, °С";
}

void JournKIV::setMeasRecord(char *file, QVector<QVariant> &outvl)
{
    MeasureStruct kivdata;
    memcpy(&kivdata, file, sizeof(MeasureStruct));
    if (kivdata.Time != 0xFFFFFFFF)
        outvl = { kivdata.NUM, TimeFunc::UnixTime32ToInvString(kivdata.Time), kivdata.Ueff[0], kivdata.Ueff[1],
            kivdata.Ueff[2], kivdata.Ieff[0], kivdata.Ieff[1], kivdata.Ieff[2], kivdata.Frequency, kivdata.U0,
            kivdata.U1, kivdata.U2, kivdata.I0, kivdata.I1, kivdata.I2, kivdata.Cbush[0], kivdata.Cbush[1],
            kivdata.Cbush[2], kivdata.Tg_d[0], kivdata.Tg_d[1], kivdata.Tg_d[2], kivdata.dCbush[0], kivdata.dCbush[1],
            kivdata.dCbush[2], kivdata.dTg_d[0], kivdata.dTg_d[1], kivdata.dTg_d[2], kivdata.Iunb, kivdata.Phy_unb,
            kivdata.Tmk, kivdata.Tamb };
}

int JournKIV::measureSize() { return sizeof(MeasureStruct); }

int JournKIV::workJournalID() { return KIVWORKJOURNID; }
