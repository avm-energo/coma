#ifndef JOURNALS_H
#define JOURNALS_H

#include "../models/etablemodel.h"

#include <QSortFilterProxyModel>

#define SYSJOURID 0
//#define WORKJOURID 3010
//#define WORKJOURIDKTF 5011
class Journals : public QObject
{
    Q_OBJECT

public:
    //    /*!
    //    Приложение 3. Номера файлов

    //        № файла	Описание
    //        1	Конфигурация
    //        3	Встроенное ПО (Firmware)
    //        4	Системный журнал
    //        5	Рабочий журнал
    //        6	Журнал измерений
    //        17	Журнал переключений
    //    */
    //    enum JournalEnum
    //    {
    //        JOURSYS = 4,
    //        JOURWORK = 5,
    //        JOURMEAS = 6
    //    };

    struct EventStruct
    {
        quint64 Time;
        quint8 EvType;
        quint8 EvNum[3];
        quint32 Reserv;
    };

    //    struct Measure
    //    {
    //        quint32 NUM;
    //        quint32 Time;
    //        float Ueff[3];
    //        float Ieff[3];
    //        float Frequency;
    //        float U0;
    //        float U1;
    //        float U2;
    //        float I0;
    //        float I1;
    //        float I2;
    //    };

    //    struct MeasureStruct : Measure
    //    {
    //        MeasureStruct(Measure meas) : Measure(meas) { }
    //        MeasureStruct() = default;
    //        float Cbush[3];
    //        float Tg_d[3];
    //        float dCbush[3];
    //        float dTg_d[3];
    //        float Iunb;
    //        float Phy_unb;
    //        float Tmk;
    //        float Tamb;
    //        quint32 res;
    //    }; // sizeof(Bda_trend_struct)=31*4=124

    //    struct MeasureStructKTF : Measure
    //    {
    //        MeasureStructKTF(Measure meas) : Measure(meas) { }
    //        MeasureStructKTF() = default;
    //        float Pf[4];
    //        float Qf[4];
    //        float Sf[4];
    //        float Cosphi;
    //        float Twin;
    //        float Tmk;
    //        float Tamb;
    //        quint32 res;
    //    }; // sizeof(Bda_trend_struct)=31*4=124
    QStringList m_sysJourDescription, m_workJourDescription, m_measJourHeaders;

    explicit Journals(QObject *parent = nullptr);
    ~Journals();

    void SetProxyModels(QSortFilterProxyModel *workmdl, QSortFilterProxyModel *sysmdl, QSortFilterProxyModel *measmdl);
    void SetJourType(int jourtype);
    void SetJourFile(const QString &jourfile);
    //    void SetParentWidget(QWidget *w);
    virtual void setWorkJourDescription() = 0;
    virtual void setMeasJourHeaders() = 0;
    virtual void setMeasRecord(char *file, QVector<QVariant> &outvl) = 0;
    virtual int measureSize() = 0;
    virtual int workJournalID() = 0;

signals:
    void Done(QString, int);
    void resendResult(int);
    void resendMaxResult(int);
    // void Fill(QVector<QVector<QVariant>>);
    void Error(QString);
    // void ModelReady(ETableModel *);
    void ReadJour(char);

private:
    // QMutex _mutex;
    ETableModel *m_sysModel, *m_workModel, *_measModel;
    QSortFilterProxyModel *_proxySysModel, *_proxyWorkModel, *_proxyMeasModel;

    int m_jourType;
    QString m_jourFile;
    const QStringList EventJourHeaders = QStringList() << " № "
                                                       << "Дата/Время UTC"
                                                       << "Описание события"
                                                       << "Тип события";
    const QStringList SysJourDescription = QStringList()
        << "Рестарт модуля"
        << "Произошла запись и переход на новую конфигурацию"
        << "Произошла запись и переход на новую версию ВПО"
        << "Произошла запись в блок Bhb (hidden block)"
        << "Произошла запись в блок Bhbm (hidden block)( Мезонин)"
        << "Произошёл отказ внешней Flash-памяти 4 Мбайт на базовой плате"
        << "Произошёл отказ внешней Flash-памяти 512 байт на мезонинной плате"
        << "Ошибка работы внешней FRAM памяти"
        << "Произошёл отказ при обновлении конфигурации"
        << "Ошибка загрузки конфигурации из flash памяти"
        << "Ошибка регулировочных коэффициентов (базовая)"
        << "Ошибка регулировочных коэффициентов (Мезонин)"
        << "Ошибка перегрев модуля"
        << "Напряжение батарейки низкое (< 2,5 В)"
        << "Ошибка связи с NTP сервером"
        << "Ошибка связи с 1PPS от антенны"
        << "Ошибка АЦП (Мезонин)"
        << "Ошибка АЦП (базовая)"
        << "Произошла запись регулировочных коэффициентов (базовая)"
        << "Произошла запись регулировочных коэффициентов (Мезонин)"
        << "Произошло стирание системного журнала"
        << "Произошло стирание рабочего журнала"
        << "Произошло стирание осциллограмм"
        << "Произошло стирание журнала измерений"
        << "Ошибка ВПО"
        << "Ошибка встроенного АЦП"
        << "Произошла запись в блок Bhb (hidden block)"
        << "Произошла запись в блок Bhbm (hidden block)( Мезонин)";

    //    const QStringList WorkJourDescription = QStringList() << "Отсутствует сигнал напряжения фазы A"
    //                                                          << "Отсутствует сигнал напряжения фазы B"
    //                                                          << "Отсутствует сигнал напряжения фазы С"
    //                                                          << "Нет реактивного тока канала А (ток меньше 2мА)"
    //                                                          << "Нет реактивного тока канала B (ток меньше 2мА)"
    //                                                          << "Нет реактивного тока канала C (ток меньше 2мА)"
    //                                                          << "Не заданы начальные значения"
    //                                                          << "Низкое напряжение фазы A"
    //                                                          << "Низкое напряжение фазы B"
    //                                                          << "Низкое напряжение фазы C"
    //                                                          << "Сигнализация по приращению тангенса дельта ввода
    //                                                          фазы А"
    //                                                          << "Сигнализация по приращению тангенса дельта ввода
    //                                                          фазы B"
    //                                                          << "Сигнализация по приращению тангенса дельта ввода
    //                                                          фазы C"
    //                                                          << "Авария по приращению тангенса дельта ввода фазы А"
    //                                                          << "Авария по приращению тангенса дельта ввода фазы B"
    //                                                          << "Авария по приращению тангенса дельта ввода фазы C"
    //                                                          << "Сигнализация по приращению C ввода фазы А"
    //                                                          << "Сигнализация по приращению C ввода фазы B"
    //                                                          << "Сигнализация по приращению C ввода фазы C"
    //                                                          << "Авария по приращению C ввода фазы А"
    //                                                          << "Авария по приращению C ввода фазы B"
    //                                                          << "Авария по приращению C ввода фазы C"
    //                                                          << "Не заданы паспортные значения"
    //                                                          << "Сигнализация по изменению небаланса тока"
    //                                                          << "Авария по изменению небаланса тока";

    //    const QStringList WorkJourDescriptionKTF = QStringList()
    //        << "Отсутствует сигнал напряжения фазы A"
    //        << "Отсутствует сигнал напряжения фазы B"
    //        << "Отсутствует сигнал напряжения фазы С"
    //        << "Отсутствует сигнал тока фазы A"
    //        << "Отсутствует сигнал тока фазы B"
    //        << "Отсутствует сигнал тока фазы С"
    //        << "Перегрузка по току фазы A"
    //        << "Перегрузка по току фазы B"
    //        << "Перегрузка по току фазы C"
    //        << "Машина включена"
    //        << "Сигнализация по опасному превышению температуры обмотки"
    //        << "Неисправны все датчики температуры об-мотки"
    //        << "Аварийное сообщение по недопустимому превышению температуры обмотки"
    //        << "Сигнализация по опасному уровню пускового тока"
    //        << "Аварийное сообщение по недопустимому уровню пускового тока";

    //    const QStringList MeasJourKTFHeaders = QStringList() << " № "
    //                                                         << "Дата/Время UTC"
    //                                                         << "Ueff фA"
    //                                                         << "Ueff фB"
    //                                                         << "Ueff фC"
    //                                                         << "Ieff фA"
    //                                                         << "Ieff фB"
    //                                                         << "Ieff фC"
    //                                                         << "Frequency"
    //                                                         << "U0"
    //                                                         << "U1"
    //                                                         << "U2"
    //                                                         << "I0"
    //                                                         << "I1"
    //                                                         << "I2"
    //                                                         << "Pf фA"
    //                                                         << "Pf фB"
    //                                                         << "Pf фC"
    //                                                         << "Pf сумм."
    //                                                         << "Qf фA"
    //                                                         << "Qf фB"
    //                                                         << "Qf фC"
    //                                                         << "Qf сумм."
    //                                                         << "Sf фA"
    //                                                         << "Sf фB"
    //                                                         << "Sf фC"
    //                                                         << "Sf сумм."
    //                                                         << "CosPhi"
    //                                                         << "Tmk,°С"
    //                                                         << "Tamb,°С"
    //                                                         << "Twin,°С";

    //    const QStringList MeasJourHeaders = QStringList() << " № "
    //                                                      << "Дата/Время UTC"
    //                                                      << "Ueff фA"
    //                                                      << "Ueff фB"
    //                                                      << "Ueff фC"
    //                                                      << "Ieff фA"
    //                                                      << "Ieff фB"
    //                                                      << "Ieff фC"
    //                                                      << "Freq"
    //                                                      << "U0"
    //                                                      << "U1"
    //                                                      << "U2"
    //                                                      << "I0"
    //                                                      << "I1"
    //                                                      << "I2"
    //                                                      << "Cbush фA"
    //                                                      << "Cbush фB"
    //                                                      << "Cbush фC"
    //                                                      << "Tg_d фA"
    //                                                      << "Tg_d фB"
    //                                                      << "Tg_d фC"
    //                                                      << "dCbush фA"
    //                                                      << "dCbush фB"
    //                                                      << "dCbush фC"
    //                                                      << "dTg_d фA"
    //                                                      << "dTg_d фB"
    //                                                      << "dTg_d фC"
    //                                                      << "Iunb"
    //                                                      << "Phy_unb"
    //                                                      << "Tmk, °С"
    //                                                      << "Tamb, °С";

    void FillEventsTable(QByteArray &ba);
    void FillMeasTable(QByteArray &ba);
    void ResultReady();

    void prepareJour(QByteArray &ba, int JourType);

public slots:
    void FillSysJour(QByteArray ba);
    void FillMeasJour(QByteArray ba);
    void FillWorkJour(QByteArray ba);
    void ReadJourFileAndProcessIt();
    void StartGetJour();
    void StartSaveJour(int jtype, QAbstractItemModel *mdl, QString filename);
};

#endif // JOURNALS_H