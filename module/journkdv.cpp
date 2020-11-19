#include "journkdv.h"

#include "../gen/timefunc.h"

JournKDV::JournKDV(QObject *parent) : Journals(parent)
{
}

void JournKDV::setWorkJourDescription()
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

void JournKDV::setMeasJourHeaders()
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
                                      << "U1"
                                      << "U2"
                                      << "I1"
                                      << "I2"
                                      << "Pf_sum"
                                      << "Qf_sum"
                                      << "Sf_sum"
                                      << "CosPhi"
                                      << "Tmk"
                                      << "Tamb"
                                      << "Twin"
                                      << "VibrSCZ_V"
                                      << "VibrSCZ_V"
                                      << "VibrSCZ_V"
                                      << "VibrSCZ_V"
                                      << "VibrSCZ_V"
                                      << "VibrSCZ_V"
                                      << "VibrSCZ_D"
                                      << "VibrSCZ_D"
                                      << "VibrSCZ_D"
                                      << "VibrSCZ_D"
                                      << "VibrSCZ_D"
                                      << "VibrSCZ_D";
}

void JournKDV::setMeasRecord(char *file, QVector<QVariant> &outvl)
{
    MeasureStruct KDVdata;
    memcpy(&KDVdata, file, sizeof(MeasureStruct));
    //    if (KDVdata.Time != 0xFFFFFFFF)
    //        outvl = { KDVdata.NUM, TimeFunc::UnixTime32ToInvString(KDVdata.Time), KDVdata.Ueff[0], KDVdata.Ueff[1],
    //            KDVdata.Ueff[2], KDVdata.Ieff[0], KDVdata.Ieff[1], KDVdata.Ieff[2], KDVdata.Frequency, KDVdata.U0,
    //            KDVdata.U1, KDVdata.U2, KDVdata.I0, KDVdata.I1, KDVdata.I2, KDVdata.Cbush[0], KDVdata.Cbush[1],
    //            KDVdata.Cbush[2], KDVdata.Tg_d[0], KDVdata.Tg_d[1], KDVdata.Tg_d[2], KDVdata.dCbush[0],
    //            KDVdata.dCbush[1], KDVdata.dCbush[2], KDVdata.dTg_d[0], KDVdata.dTg_d[1], KDVdata.dTg_d[2],
    //            KDVdata.Iunb, KDVdata.Phy_unb, KDVdata.Tmk, KDVdata.Tamb };
}

int JournKDV::measureSize()
{
    return sizeof(MeasureStruct);
}

int JournKDV::workJournalID()
{
    return KDVWORKJOURNID;
}
