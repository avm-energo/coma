#ifndef JOURNALDIALOG_H
#define JOURNALDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include "../gen/maindef.h"
#include "../widgets/etableview.h"
#include "../widgets/etablemodel.h"
#include "../gen/s2.h"


#define MAXSWJNUM   262144

#define SYSJOURID   0
#define WORKJOURID  3010

class JournalDialog : public QDialog
{
    Q_OBJECT
public:
    JournalDialog();

    enum JournalEnum
    {
        JOURSYS     = 0,
        JOURWORK    = 1,
        JOURMEAS    = 2
    };

    struct EventStruct{
    quint64 Time;
    quint8 EvType;
    quint8 EvNum[3];
    quint32 Reserv;
    };

    struct MeasureStruct{
    quint32	NUM;
    quint32 Time;
    float Ueff[3];
    float Ieff[3];
    float Frequency;
    float U0;
    float U1;
    float U2;
    float I0;
    float I1;
    float I2;
    float Cbush[3];
    float Tg_d[3];
    float dCbush[3];
    float dTg_d[3];
    float Iunb;
    float Phy_unb;
    float Tmk;
    float Tamb;
    quint32 res;
    };	// sizeof(Bda_trend_struct)=31*4=124

    quint8 start;
//    quint64 LTime;
    int SaveI;
/*    QVector<S2::DataRec>* SaveWJour;
    QVector<S2::DataRec>* SaveSJour;
    QVector<S2::DataRec>* SaveMJour; */

private:
    const QStringList SysJourDescription = QStringList() << "Рестарт модуля"
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
                                            << "Произошла запись регулировочные коэффициенты (базовая)"
                                            << "Произошла запись регулировочные коэффициенты (Мезонин)"
                                            << "Произошло стирание системного журнала"
                                            << "Произошло стирание рабочего журнала"
                                            << "Произошло стирание осциллограмм"
                                            << "Произошло стирание журнала измерений"
                                            << "Ошибка ВПО"
                                            << "Ошибка встроенного АЦП"
                                            << "Произошла запись в блок Bhb (hidden block)"
                                            << "Произошла запись в блок Bhbm (hidden block)( Мезонин)";

    const QStringList WorkJourDescription = QStringList() << "Отсутствует сигнал напряжения фазы A"
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

    void SetupUI();
    void FillEventsTable(char *file, int jourtype);
    void FillMeasTable(char *file);
    int GetJourNum(const QString &objname);

signals:
    void ReadJour(char);

private slots:
    void GetJour();
    void GetJourMeasj();
    void ReadJourFileAndProcessIt();
    void JourFileChoosed(QString &file);
    void EraseJour();
    void SaveJour();

public slots:
    void FillSysJour(QByteArray ba);
    void FillMeasJour(QByteArray ba);
    void FillWorkJour(QByteArray ba);

private:
    QString JourFile;
    int JourType;
};

#endif // JOURNALDIALOG_H
